#include <modules/image/png.h>

PNGEncoderModule::PNGEncoderModule(const char* cover_file_path) : PNGModule(cover_file_path) {
	//do nothing if there is no secret provided, just load the BMP into memory
	printf("Created new PNGEncoderModule\n");
	
	make_crc_table();
}
PNGEncoderModule::PNGEncoderModule(const char* cover_file_path, const char* secret_file_path) : PNGModule(cover_file_path) {
	TRY(utils::load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));

	make_crc_table();
}
PNGEncoderModule::PNGEncoderModule(const char* cover_file_path, const char* secret_file_path, const PNGModuleOptions& steg_options) : PNGModule(cover_file_path) {
	TRY(utils::load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));

	make_crc_table();

	launch_steganos(steg_options);
}

PNGEncoderModule::~PNGEncoderModule() {
	//delete the secret data allocated memory
	if (secret_data != nullptr)
		delete[] secret_data;

	if (secret_stream != nullptr)
		delete secret_stream;
}

error_code PNGEncoderModule::load_secret(const char* secret_file_path) {
	TRY(utils::load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));

	return error_code::NONE;
}

void PNGEncoderModule::make_crc_table() {
	uint32_t c;
	int32_t n, k;

	for (n = 0; n < 256; n++) {
		c = (uint32_t)n;
		for (k = 0; k < 8; k++) {
			if (c & 1)
				c = 0xedb88320L ^ (c >> 1);
			else
				c = c >> 1;
		}
		crc_table[n] = c;
	}
}

uint32_t PNGEncoderModule::update_crc(uint32_t crc, uint8_t* buf, int32_t len) {
	uint32_t c = crc;
	int32_t n;

	for (n = 0; n < len; n++) {
		c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
	}

	return c;
}
uint32_t PNGEncoderModule::crc(uint8_t* buf, int32_t len) {
	return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}

error_code PNGEncoderModule::simple_sequential_embed_handler(const PNGModuleOptions& steg_options) {
	PNGMetadataStruct metadata = get_metadata();
	uint64_t bytes_written_so_far = 0, successful_written_bytes = 0;

	//try writing the raw bytes of the secret file
	for (PNGChunkStruct& png_chunk : png_chunks) {
		char bytes[4];
		convert_chunk_type_to_string(bytes, png_chunk.type);
		if (strncmp(bytes, "IDAT", 4) != 0)
			continue;

		uint8_t pixel_size = 0;
		if (metadata.color_type == 0x06) { //pixel type is rgba
			pixel_size = 4;
		}
		else if (metadata.color_type == 0x02) {
			pixel_size = 3;
		}

		uint32_t how_many_bytes_to_write = 0;
		if (png_chunk.length > (secret_data_size - bytes_written_so_far))
			how_many_bytes_to_write = secret_data_size - bytes_written_so_far;
		else
			how_many_bytes_to_write = png_chunk.length / 8;
		
		error_code sequential_result = simple_sequential_embed(
			png_chunk.length - 6, // - 2 bytes at the beginning for the flags - 4 bytes at the end for the check value
			png_chunk.data + 2,
			how_many_bytes_to_write,
			secret_data + bytes_written_so_far,
			successful_written_bytes
		);

		uint8_t* copy_of_fields_for_crc = new uint8_t[(size_t)png_chunk.length + 4];
		for (uint8_t i = 0; i < 4; i++) //to compute the new crc, we need the chunk type and chunk data
			copy_of_fields_for_crc[i] = bytes[i];
		memcpy_s(copy_of_fields_for_crc + 4, png_chunk.length, png_chunk.data, png_chunk.length);
		png_chunk.crc = crc(copy_of_fields_for_crc, png_chunk.length + 4);
		delete[] copy_of_fields_for_crc;

		if (sequential_result == error_code::NONE)
			return error_code::NONE;
		else if (sequential_result != error_code::INSUFFICIENT_COVER_SPACE) {
			printf("Got big error while doing sequential error\n");
			exit(-1);
		}
			
		bytes_written_so_far += successful_written_bytes;

	}

	return error_code::MISC_ERROR;
};

error_code PNGEncoderModule::personal_scramble_embed_handler(const PNGModuleOptions& steg_options) {
	printf("Secret data size = %llu\n", (uint64_t)secret_data_size);

	return error_code::NONE;
};


error_code PNGEncoderModule::launch_steganos(const PNGModuleOptions& steg_options) {
	if (steg_options.compress_secret)
		utils::compress_data(secret_data);

	switch (steg_options.algorithm)
	{
	case PNGModuleSupportedAlgorithms::SEQUENTIAL:
		TRY(simple_sequential_embed_handler(steg_options));
		break;
	case PNGModuleSupportedAlgorithms::PERSONAL_SCRAMBLE:
		TRY(personal_scramble_embed_handler(steg_options));
		break;
	default:
		std::cout << "Algorithm not yet implemented, its on my TO DO\n";
		return error_code::MISC_ERROR;
	}


	TRY(write_png());

	return error_code::NONE;
}
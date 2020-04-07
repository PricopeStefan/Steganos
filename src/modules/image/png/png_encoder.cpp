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
	error_code sequential_result = simple_sequential_embed(
		image_size,
		image_data,
		secret_data_size,
		secret_data,
		0
	);

	auto metadata = get_metadata();
	std::vector<uint8_t> image_copy(image_data, image_data + image_size);
	unsigned error = lodepng::encode(steg_options.output_filename + ".png", image_copy, metadata.width, metadata.height);
	if (error)
		return error_code::STREAM_ERROR;

	return error_code::NONE;
}


error_code PNGEncoderModule::launch_steganos(const PNGModuleOptions& steg_options) {
	if (steg_options.compress_secret)
		utils::compress_data(secret_data);

	switch (steg_options.algorithm)
	{
	case PNGModuleSupportedAlgorithms::SEQUENTIAL:
		TRY(simple_sequential_embed_handler(steg_options));
		break;
	default:
		std::cout << "Algorithm not yet implemented, its on my TO DO\n";
		return error_code::MISC_ERROR;
	}

	return error_code::NONE;
}
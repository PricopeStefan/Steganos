#include <modules/image/png.h>



#ifdef __linux__ 
#include <arpa/inet.h>
#include <string.h>
#include <memory.h>
#elif _WIN32
#include <WinSock2.h>
#endif

PNGModule::PNGModule(const char* png_file_path) {
	this->png_file_path = png_file_path;
	TRY(utils::load_stream(png_file_path, png_stream));

	TRY(read_cover_data());

	auto metadata = get_metadata();
	std::vector<uint8_t> image_data_as_vector; //the raw pixels
	unsigned error = lodepng::decode(image_data_as_vector, metadata.width, metadata.height, png_file_path);
	if (error) {
		printf("couldn't decode file\n");
		return;
	}

	image_size = image_data_as_vector.size();
	image_data = new uint8_t[image_size];
	memcpy(image_data, &image_data_as_vector[0], image_size);
	//we now have our image data as a raw byte stream in memory

}

PNGModule::~PNGModule() {
	for (const PNGChunkStruct& png_chunk : png_chunks) {
		if (png_chunk.data != nullptr)
			delete[] png_chunk.data;
	}

	if (png_stream != nullptr)
		delete png_stream;

	if (image_data != nullptr)
		delete[] image_data;
}

void PNGModule::convert_chunk_type_to_string(char(&buf)[4], uint32_t uval) const
{
	buf[0] = (uval >> 24) & 0xFF;
	buf[1] = (uval >> 16) & 0xFF;
	buf[2] = (uval >> 8) & 0xFF;
	buf[3] = uval & 0xFF;
}

error_code PNGModule::read_cover_data() {
	if (png_stream == nullptr || !png_stream->is_open())
		return error_code::STREAM_ERROR;

	//read the first 13 bytes of the stream to check if they are the header of a png file
	char first_bytes[sizeof(PNG_HEADER) - 1];
	png_stream->read(first_bytes, sizeof(PNG_HEADER) - 1);

	if (strncmp(first_bytes, PNG_HEADER, 8) != 0)
		return error_code::COVER_FILE_ERROR; //not a png file

	uint32_t index = 0;
	while (true) {
		uint32_t chunk_length = 0, chunk_type = 0, chunk_crc = 0;
		png_stream->read(reinterpret_cast<char*>(&chunk_length), sizeof(chunk_length));
		png_stream->read(reinterpret_cast<char*>(&chunk_type), sizeof(chunk_type));
		chunk_length = ntohl(chunk_length);
		chunk_type = ntohl(chunk_type);

		char bytes[4];
		convert_chunk_type_to_string(bytes, chunk_type);
		index++;

		uint8_t* chunk_data = new uint8_t[chunk_length];
		png_stream->read(reinterpret_cast<char*>(chunk_data), chunk_length);
		png_stream->read(reinterpret_cast<char*>(&chunk_crc), sizeof(chunk_crc));
		chunk_crc = ntohl(chunk_crc);

		printf("[Chunk %u] Type = %c%c%c%c ; length = %u; CRC = %u\n", index, bytes[0], bytes[1], bytes[2], bytes[3], chunk_length, chunk_crc);

		png_chunks.push_back(PNGChunkStruct(chunk_length, chunk_type, chunk_data, chunk_crc));
		if (strncmp(bytes, "IEND", 4) == 0)
			break;
	}

	return error_code::NONE;
}

error_code PNGModule::write_png(const char* output_path) {
	std::ofstream embedded_stream(output_path, std::ios_base::binary);
	if (!embedded_stream.is_open())
		return error_code::STREAM_ERROR;

	//TO DO : check if writes succedded
	embedded_stream.write(PNG_HEADER, sizeof(PNG_HEADER) - 1); //without the \0


	for (const PNGChunkStruct& png_chunk : png_chunks) {
		uint32_t type = htonl(png_chunk.type), length = htonl(png_chunk.length), crc = htonl(png_chunk.crc);
		embedded_stream.write(reinterpret_cast<const char*>(&length), sizeof(length));
		embedded_stream.write(reinterpret_cast<const char*>(&type), sizeof(type));
		embedded_stream.write(reinterpret_cast<char*>(png_chunk.data), png_chunk.length);
		embedded_stream.write(reinterpret_cast<const char*>(&crc), sizeof(crc));
	}


	embedded_stream.close();

	return error_code::NONE;
}

const PNGMetadataStruct PNGModule::get_metadata() const {
	if (png_chunks.size() == 0) //no chunks loaded
		return PNGMetadataStruct();

	char bytes[4];
	convert_chunk_type_to_string(bytes, png_chunks.at(0).type);
	if (strncmp(bytes, "IHDR", 4) != 0)
		return PNGMetadataStruct(); //no IHDR chunk as the first chunk of the png

	PNGMetadataStruct metadata;
	
	#ifdef __linux__ 
		memcpy(&metadata, (void*)png_chunks.at(0).data, sizeof(PNGMetadataStruct));
	#elif _WIN32
		std::memcpy(&metadata, (void*)png_chunks.at(0).data, sizeof(PNGMetadataStruct));
	#endif

	metadata.height = ntohl(metadata.height);
	metadata.width = ntohl(metadata.width);

	return metadata;
}


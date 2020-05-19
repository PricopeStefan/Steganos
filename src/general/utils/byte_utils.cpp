#include <general/utils.h>

error_code utils::load_stream(const char* file_path, std::ifstream*& stream, std::ios_base::openmode stream_options) {
	if (stream != nullptr) {
		//stream is already open
		return error_code::STREAM_ERROR;
	}

	stream = new std::ifstream(file_path, stream_options);
	if (stream->is_open()) {
		return error_code::NONE;
	}

	//the opening of the stream failed, deleting it
	delete stream;
	stream = nullptr;

	return error_code::STREAM_ERROR;
}

error_code utils::read_byte_stream(std::ifstream* stream, uint8_t*& byte_stream, uint32_t& stream_size) {
	stream->seekg(0, std::ios::end);
	std::streamsize file_size = stream->tellg();
	stream->seekg(0, std::ios::beg);

	byte_stream = new uint8_t[file_size + 4];
	if (stream->read(reinterpret_cast<char*>(byte_stream + 4), file_size)) {
		stream_size = (uint32_t)(file_size + 4);
		//adding the stream size info to the byte stream as the first 4 bytes
		byte_stream[0] = ((uint8_t*)&stream_size)[0];
		byte_stream[1] = ((uint8_t*)&stream_size)[1];
		byte_stream[2] = ((uint8_t*)&stream_size)[2];
		byte_stream[3] = ((uint8_t*)&stream_size)[3];
		printf("The first 4 bytes are %02x %02x %02x %02x\n", byte_stream[0], byte_stream[1], byte_stream[2], byte_stream[3]);
		//succesfully read all the secret file bytes
		return error_code::NONE;

		//TO DO : add some statistics that can tell if there is enough space 
		//to write all the secret data into the cover file or not
	}

	delete byte_stream;
	stream_size = 0;
	return error_code::STREAM_ERROR;
}

int32_t utils::compress_data(uint8_t*& data_bytes) {
	return 0;
}

void utils::setLSB(uint8_t& value, const bool& bit) {
	if (bit)
		value |= 1;
	else value &= 0xFE;
}
void utils::setLSB(uint16_t& value, const bool& bit) {
	if (bit)
		value |= 1;
	else value &= 0xFFFE;
}
void utils::setLSB(uint32_t& value, const bool& bit) {
	if (bit)
		value |= 1;
	else value &= 0xFFFFFFFE;
}

uint8_t utils::read_byte_from_lsbs(uint8_t* byte_stream, uint64_t byte_stream_size) {
	if (byte_stream_size < 8)
		return 0;

	uint8_t byte = 0;
	for (short index = 0; index < 8; index++) {
		byte |= (byte_stream[index] % 2) << index;
	}

	return byte;
}

uint32_t utils::convert_synchsafe_uint_to_normal_uint(uint32_t synch_safe) {
	uint8_t bytes[4];
	bytes[0] = (synch_safe >> 24) & 0xFF;
	bytes[1] = (synch_safe >> 16) & 0xFF;
	bytes[2] = (synch_safe >> 8) & 0xFF;
	bytes[3] = synch_safe & 0xFF;

	uint32_t byte0 = bytes[3];
	uint32_t byte1 = bytes[2];
	uint32_t byte2 = bytes[1];
	uint32_t byte3 = bytes[0];

	return byte0 << 21 | byte1 << 14 | byte2 << 7 | byte3;
}
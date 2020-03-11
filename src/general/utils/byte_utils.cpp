#include <general/utils.h>

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
		printf("Writing bytes %02x %02x %02x %02x\n", byte_stream[0], byte_stream[1], byte_stream[2], byte_stream[3]);
		//succesfully read all the secret file bytes
		return error_code::NONE;

		//TO DO : add some statistics that can tell if there is enough space 
		//to write all the secret data into the cover file or not
	}

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

uint8_t utils::read_byte_from_lsbs(uint8_t* byte_stream, uint64_t byte_stream_size) {
	if (byte_stream_size < 8)
		return 0;

	uint8_t byte = 0;
	for (short index = 0; index < 8; index++) {
		byte |= (byte_stream[index] % 2) << index;
	}

	return byte;
}

#include <general/utils.h>

#include <modules/image/bmp.h>


Module* utils::modules::module_picker(const char* cover_file_path, bool for_embedding) {
	Module* best_module = nullptr;
	std::ifstream cover_stream(cover_file_path, std::ios::binary);

	uint8_t byte_buffer[8]; //usually 8 bytes is enough to determine the MIME type of the cover file
	cover_stream.read(reinterpret_cast<char*>(byte_buffer), 8);

	//BMP file magic bytes based on https://en.wikipedia.org/wiki/BMP_file_format
	if (byte_buffer[0] == 'B' && byte_buffer[1] == 'M') {
		if (for_embedding)
			best_module = new BMPEncoderModule(cover_file_path);
		else
			best_module = new BMPDecoderModule(cover_file_path);
	}

	cover_stream.close();
	return best_module;
}
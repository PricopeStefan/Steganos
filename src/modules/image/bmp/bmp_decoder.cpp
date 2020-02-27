#include <modules/image/bmp.h>

error_code BMPDecoderModule::write_secret(const char* output_path) const {
	if (secret_data_size == 0 || secret_data == nullptr) {
		return error_code::DATA_ERROR;
	}

	std::ofstream embedded_stream(output_path, std::ios_base::binary);
	if (!embedded_stream.is_open() || embedded_stream.fail()) {
		return error_code::STREAM_ERROR;
	}

	//TO DO : check if writes succedded
	embedded_stream.write(reinterpret_cast<char*>(secret_data), secret_data_size);
	embedded_stream.close();

	return error_code::NONE;
}

error_code BMPDecoderModule::sequential_handler(const BMPModuleOptions& steg_options) {
	size_t secret_data_byte_index = 0;
	uint8_t bit_index = 0;
	uint8_t current_byte = 0;

	//to do: this algorithm is pretty generic, move it somewhere more global
	for (uint32_t line_index = 0; line_index < cover_image_metadata.height; line_index++) {
		for (uint32_t col_index = 0; col_index < cover_image_metadata.width; col_index++) {
			BGRPixel& pixel = cover_image_data[line_index][col_index];

			for (uint8_t color_index = 0; color_index < 3; color_index++) {
				//change the LSB to reflect the current bit of the secret data
				uint8_t& color_level = color_index == 0 ? pixel.red : color_index == 1 ? pixel.green : pixel.blue;

				//go to the next bit
				bit_index++;

				current_byte |= (color_level & 1) << 7;
				if (bit_index < 8)
					current_byte >>= 1;
				else if (bit_index == 8) {
					//check to see if done with the writing of the secret data
					if (secret_data_byte_index > 0 && secret_data_byte_index == secret_data_size) {
						goto writing_output;
					}
					else {
						//printf("Current byte is %c\n", current_byte);
						bit_index = 0;
						secret_data[secret_data_byte_index] = current_byte;
						current_byte = 0;
						secret_data_byte_index++;

						//done reading the first 4 bytes that are for the image size
						if (secret_data_byte_index == 4 && secret_data_size == 0) {
							bit_index = 0;
							secret_data_size = *((uint32_t*)secret_data);
							std::cout << "Total hidden data is " << secret_data_size << std::endl;

							delete[] secret_data;
							//there is a possibility no data is encoded
							if (secret_data_size == 0) {
								goto writing_output;
							}

							secret_data_byte_index = 0;
							secret_data = new uint8_t[secret_data_size];
						}
					}
					//printf("Current byte = %c\n", current_byte);
				}
			}

		}
	}

	//exit loop label
writing_output:
	TRY(write_secret());

	return error_code::NONE;
}

BMPDecoderModule::BMPDecoderModule(const char* embedded_path) : BMPModule(embedded_path) {
	//do nothing if no options are given, just load the bmp into memory
}
BMPDecoderModule::BMPDecoderModule(const char* embedded_path, const BMPModuleOptions steg_options) : BMPModule(embedded_path) {
	//image is loaded, try and decode it
	TRY(launch_steganos(steg_options));
}
BMPDecoderModule::~BMPDecoderModule() {
	delete[] secret_data;
}


error_code BMPDecoderModule::launch_steganos(const BMPModuleOptions& steg_options) {
	switch (steg_options.algorithm) {
	case BMPModuleSupportedAlgorithms::SEQUENTIAL:
		return sequential_handler(steg_options);
	default:
		break;
	}

	return error_code::NONE;
}
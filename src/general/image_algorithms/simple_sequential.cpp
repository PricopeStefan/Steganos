#include <general/image_algorithms.h>

using BGRPixel = utils::pixels::BGRPixel;
using RGBPixel = utils::pixels::RGBPixel;

//given a simple MxN matrix of bytes representing the image pixels data 
//and a byte stream representing the data of the file which will be hidden
//replace every last bit of each of the pixel's channels with the bits from the secret byte stream
//until either the entire message is written or we run out of pixels
error_code simple_sequential_embed(uint32_t image_width,
									uint32_t image_height,
									BGRPixel**& image_data,
									uint32_t byte_stream_size, 
									uint8_t*& byte_stream)
{
	uint32_t secret_data_byte_index = 0;
	uint8_t bit_index = 0;
	uint8_t current_byte = byte_stream[secret_data_byte_index];

	//helper lambda function to advance to the next data bit from the secret stream
	//returns how many bytes there are left to write
	auto advance_to_next_bit = [&]() {
		//go to the next bit
		bit_index++;
		current_byte >>= 1;

		if (bit_index == 8) {
			secret_data_byte_index++;
			//check to see if done with the writing of the secret data
			if (secret_data_byte_index == byte_stream_size)
				return 0u; //no bits left to write, return 0

			bit_index = 0;
			current_byte = byte_stream[secret_data_byte_index];
			//printf("Current byte = %c\n", current_byte);
		}

		//there are still byte_stream_size - secret_data_byte_index bytes left to write
		return (byte_stream_size - secret_data_byte_index);
	};

	for (uint32_t line_index = 0; line_index < image_height; line_index++) {
		for (uint32_t col_index = 0; col_index < image_width; col_index++) {
			utils::pixels::BGRPixel& pixel = image_data[line_index][col_index];

			for (uint8_t color_index = 0; color_index < 3; color_index++) {
				//change the LSB to reflect the current bit of the secret data
				uint8_t& color_level = color_index == 0 ? pixel.red : color_index == 1 ? pixel.green : pixel.blue;
				utils::setLSB(color_level, current_byte & 1);

				if (advance_to_next_bit() == 0)
					return error_code::NONE;
			}
		}
	}

	return error_code::INSUFFICIENT_COVER_SPACE;
}


error_code simple_sequential_embed(uint32_t image_width,
									uint32_t image_height,
									RGBPixel**& image_data,
									uint32_t byte_stream_size,
									uint8_t*& byte_stream)
{
	uint32_t secret_data_byte_index = 0;
	uint8_t bit_index = 0;
	uint8_t current_byte = byte_stream[secret_data_byte_index];

	//helper lambda function to advance to the next data bit from the secret stream
	//returns how many bytes there are left to write
	auto advance_to_next_bit = [&]() {
		//go to the next bit
		bit_index++;
		current_byte >>= 1;

		if (bit_index == 8) {
			secret_data_byte_index++;
			//check to see if done with the writing of the secret data
			if (secret_data_byte_index == byte_stream_size)
				return 0u; //no bits left to write, return 0

			bit_index = 0;
			current_byte = byte_stream[secret_data_byte_index];
			//printf("Current byte = %c\n", current_byte);
		}

		//there are still byte_stream_size - secret_data_byte_index bytes left to write
		return (byte_stream_size - secret_data_byte_index);
	};

	for (uint32_t line_index = 0; line_index < image_height; line_index++) {
		for (uint32_t col_index = 0; col_index < image_width; col_index++) {
			utils::pixels::RGBPixel& pixel = image_data[line_index][col_index];

			for (uint8_t color_index = 0; color_index < 3; color_index++) {
				//change the LSB to reflect the current bit of the secret data
				uint8_t& color_level = color_index == 0 ? pixel.red : color_index == 1 ? pixel.green : pixel.blue;
				utils::setLSB(color_level, current_byte & 1);

				if (advance_to_next_bit() == 0)
					return error_code::NONE;
			}
		}
	}

	return error_code::INSUFFICIENT_COVER_SPACE;
}

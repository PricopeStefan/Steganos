#include <general/image_algorithms.h>

using BGRPixel = utils::pixels::BGRPixel;
using RGBPixel = utils::pixels::RGBPixel;

error_code simple_sequential_embed(uint64_t image_data_size,
	uint8_t*& image_data,
	uint32_t byte_stream_size,
	uint8_t*& byte_stream)

{
	uint32_t secret_data_byte_index = 0;
	uint8_t bit_index = 0;
	uint8_t current_byte = byte_stream[secret_data_byte_index];
	//printf("Current byte = %02X\n", current_byte);

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
			//printf("Current byte = %02X\n", current_byte);
		}

		//there are still byte_stream_size - secret_data_byte_index bytes left to write
		return (byte_stream_size - secret_data_byte_index);
	};

	for (uint32_t current_byte_index = 0; current_byte_index < image_data_size; current_byte_index++) {
			uint8_t& image_data_byte = image_data[current_byte_index];

			utils::setLSB(image_data_byte, current_byte & 1);

			if (advance_to_next_bit() == 0)
				return error_code::NONE;
	}

	return error_code::INSUFFICIENT_COVER_SPACE;
}



error_code simple_sequential_decode(uint64_t image_data_size,
	uint8_t*& image_data,
	uint32_t& secret_data_size,
	uint8_t*& secret_data)

{
	if (image_data_size < 32)
		return error_code::COVER_FILE_ERROR;

	uint64_t image_data_index = 0;
	//reading the first 32 bytes from the image data to get the embedded secret size
	uint8_t byte4 = utils::read_byte_from_lsbs(image_data + image_data_index, image_data_size - image_data_index); image_data_index += 8;
	uint8_t byte3 = utils::read_byte_from_lsbs(image_data + image_data_index, image_data_size - image_data_index); image_data_index += 8;
	uint8_t byte2 = utils::read_byte_from_lsbs(image_data + image_data_index, image_data_size - image_data_index); image_data_index += 8;
	uint8_t byte1 = utils::read_byte_from_lsbs(image_data + image_data_index, image_data_size - image_data_index); image_data_index += 8;
	secret_data_size = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;

	if (secret_data_size < 4)
		return error_code::COVER_FILE_ERROR;
	//the size included the first 4 bytes which were used as size info
	secret_data_size -= 4;

	secret_data = new uint8_t[secret_data_size];

	for (uint32_t secret_data_index = 0; secret_data_index < secret_data_size; secret_data_index++) {
		uint8_t secret_byte = utils::read_byte_from_lsbs(image_data + image_data_index, image_data_size - image_data_index);
		image_data_index += 8;

		secret_data[secret_data_index] = secret_byte;

		if (image_data_size < 8)
			return error_code::COVER_FILE_ERROR;
	}

	return error_code::NONE;
}




//given a simple MxN matrix of bytes representing the image pixels data 
//and a byte stream representing the data of the file which will be hidden
//replace every last bit of each of the pixel's channels with the bits from the secret byte stream
//until either the entire message is written or we run out of pixels
//error_code simple_sequential_embed(uint32_t image_width,
//									uint32_t image_height,
//									BGRPixel**& image_data,
//									uint32_t byte_stream_size, 
//									uint8_t*& byte_stream)
//{
//	uint32_t secret_data_byte_index = 0;
//	uint8_t bit_index = 0;
//	uint8_t current_byte = byte_stream[secret_data_byte_index];
//
//	//helper lambda function to advance to the next data bit from the secret stream
//	//returns how many bytes there are left to write
//	auto advance_to_next_bit = [&]() {
//		//go to the next bit
//		bit_index++;
//		current_byte >>= 1;
//
//		if (bit_index == 8) {
//			secret_data_byte_index++;
//			//check to see if done with the writing of the secret data
//			if (secret_data_byte_index == byte_stream_size)
//				return 0u; //no bits left to write, return 0
//
//			bit_index = 0;
//			current_byte = byte_stream[secret_data_byte_index];
//			//printf("Current byte = %c\n", current_byte);
//		}
//
//		//there are still byte_stream_size - secret_data_byte_index bytes left to write
//		return (byte_stream_size - secret_data_byte_index);
//	};
//
//	for (uint32_t line_index = 0; line_index < image_height; line_index++) {
//		for (uint32_t col_index = 0; col_index < image_width; col_index++) {
//			utils::pixels::BGRPixel& pixel = image_data[line_index][col_index];
//
//			for (uint8_t color_index = 0; color_index < 3; color_index++) {
//				//change the LSB to reflect the current bit of the secret data
//				uint8_t& color_level = color_index == 0 ? pixel.red : color_index == 1 ? pixel.green : pixel.blue;
//				utils::setLSB(color_level, current_byte & 1);
//
//				if (advance_to_next_bit() == 0)
//					return error_code::NONE;
//			}
//		}
//	}
//
//	return error_code::INSUFFICIENT_COVER_SPACE;
//}
//
//
//error_code simple_sequential_embed(uint32_t image_width,
//									uint32_t image_height,
//									RGBPixel**& image_data,
//									uint32_t byte_stream_size,
//									uint8_t*& byte_stream)
//{
//	uint32_t secret_data_byte_index = 0;
//	uint8_t bit_index = 0;
//	uint8_t current_byte = byte_stream[secret_data_byte_index];
//
//	//helper lambda function to advance to the next data bit from the secret stream
//	//returns how many bytes there are left to write
//	auto advance_to_next_bit = [&]() {
//		//go to the next bit
//		bit_index++;
//		current_byte >>= 1;
//
//		if (bit_index == 8) {
//			secret_data_byte_index++;
//			//check to see if done with the writing of the secret data
//			if (secret_data_byte_index == byte_stream_size)
//				return 0u; //no bits left to write, return 0
//
//			bit_index = 0;
//			current_byte = byte_stream[secret_data_byte_index];
//			//printf("Current byte = %c\n", current_byte);
//		}
//
//		//there are still byte_stream_size - secret_data_byte_index bytes left to write
//		return (byte_stream_size - secret_data_byte_index);
//	};
//
//	for (uint32_t line_index = 0; line_index < image_height; line_index++) {
//		for (uint32_t col_index = 0; col_index < image_width; col_index++) {
//			utils::pixels::RGBPixel& pixel = image_data[line_index][col_index];
//
//			for (uint8_t color_index = 0; color_index < 3; color_index++) {
//				//change the LSB to reflect the current bit of the secret data
//				uint8_t& color_level = color_index == 0 ? pixel.red : color_index == 1 ? pixel.green : pixel.blue;
//				utils::setLSB(color_level, current_byte & 1);
//
//				if (advance_to_next_bit() == 0)
//					return error_code::NONE;
//			}
//		}
//	}
//
//	return error_code::INSUFFICIENT_COVER_SPACE;
//}
//
//

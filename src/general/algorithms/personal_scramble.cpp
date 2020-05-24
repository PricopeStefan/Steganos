#include <general/image_algorithms.h>

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <random>   


std::vector<uint8_t> get_permutation_order(uint8_t size, size_t seed_from_hash);

// the first 8x8 block of the image is going to contain metadata(secret message length etc.)
// this functions embeds that metadata into the byte stream
error_code embed_metadata(utils::pixels::types pixel_type, uint32_t width, uint8_t*& image_data, uint32_t byte_stream_size) {
	if (pixel_type == utils::pixels::types::RGB || pixel_type == utils::pixels::types::BGR) {
		//working on 8x8 grids, when we typed the first 24/32 bits we will go and type the remaining 8 bits on the second line
		for (short bit_index = 0; bit_index < 32; bit_index++) {
			uint8_t& byte_to_be_rewritten = image_data[3 * width * (bit_index / 24) + bit_index % 24];
			utils::setLSB(byte_to_be_rewritten, (byte_stream_size >> bit_index) & 1);
		}
	}
	else {
		for (short bit_index = 0; bit_index < 32; bit_index++) {
			uint8_t& byte_to_be_rewritten = image_data[bit_index];
			utils::setLSB(byte_to_be_rewritten, (byte_stream_size >> bit_index) & 1);
		}
	}

	return error_code::NONE;
}

error_code personal_scramble_embed(utils::pixels::types pixel_type,
									uint32_t height,
									uint32_t width,
									uint8_t*& image_data,
									uint32_t byte_stream_size,
									uint8_t*& byte_stream,
									std::string password) 
{
	uint32_t secret_data_byte_index = 4;
	uint8_t bit_index = 0;
	uint8_t current_byte = byte_stream[secret_data_byte_index];
	uint8_t pixel_size = static_cast<uint8_t>(pixel_type);
	size_t message_hash_used_as_seed = std::hash<std::string>{}(password);
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
	printf("Pixel size = %d\n", int(pixel_size));

	//we know the first 32 bytes contain the 4 bytes that determine the embedded file length
	//for those 32 bytes 10 pixels and 2 color channels from 11th pixel are needed(if pixel type is RGB)
	//for those 32 bytes 8 pixels are needed(if pixel type is RGBA)
	TRY(embed_metadata(pixel_type, width, image_data, byte_stream_size));

	//effectively embedding the data
	//IGNORE THE FIRST 4 BYTES OF BYTE_STREAM(THEY ARE THE BYTE_STREAM LENGTH)
	for (uint32_t line_index = 0; line_index < height; line_index += 8) {
		for (uint32_t col_index = 0; col_index < width; col_index += 8) {
			if (line_index == 0 && col_index == 0)
				continue; //ignore the first 8x8 pixel block of the image, because thats the metadata block

			uint8_t block_width = (width - col_index) > 8 ? 8 : (width - col_index);
			uint8_t block_height = (height - line_index) > 8 ? 8 : (height - line_index);
			auto order = get_permutation_order(block_width * block_height, message_hash_used_as_seed); //maybe change the hash each time?

			for (auto relative_inner_pixel_index : order) {
				uint32_t absolute_pixel_line = line_index + relative_inner_pixel_index / block_width;
				uint32_t absolute_pixel_column = col_index + relative_inner_pixel_index % block_width;

				for (uint8_t pixel_byte_index = 0; pixel_byte_index < pixel_size; pixel_byte_index++) {
					uint8_t* image_data_byte = &image_data[pixel_size * width * absolute_pixel_line + pixel_size * absolute_pixel_column + pixel_byte_index];

					if (byte_stream[secret_data_byte_index] == 0xdd && byte_stream[secret_data_byte_index + 1] == 0xbe && byte_stream[secret_data_byte_index + 2] == 0xdf) {
						printf("Relative = %d written to [%d-%d aka index %d]\n", relative_inner_pixel_index, absolute_pixel_line, absolute_pixel_column, pixel_size * width * absolute_pixel_line + pixel_size * absolute_pixel_column + pixel_byte_index);
					}

					utils::setLSB(*image_data_byte, current_byte & 1);
					if (advance_to_next_bit() == 0)
						return error_code::NONE;
				}
			}
		}
	}

	return error_code::INSUFFICIENT_COVER_SPACE;
}




//decode functions for the personal scramble algorithm

error_code decode_metadata(utils::pixels::types pixel_type, uint32_t width, uint8_t*& image_data, uint32_t& byte_stream_size) {
	byte_stream_size = 0;

	if (pixel_type == utils::pixels::types::RGB || pixel_type == utils::pixels::types::BGR) {
		//working on 8x8 grids, when we typed the first 24/32 bits we will go and type the remaining 8 bits on the second line
		for (short bit_index = 0; bit_index < 32; bit_index++) {
			uint8_t& source_byte = image_data[3 * width * (bit_index / 24) + bit_index % 24];
			byte_stream_size |= (source_byte % 2) << bit_index;
		}
	}
	else {
		for (short bit_index = 0; bit_index < 32; bit_index++) {
			uint8_t& source_byte = image_data[bit_index];
			byte_stream_size |= (source_byte % 2) << bit_index;
		}
	}
	

	printf("[PersonalScramble][LittleEndianSize] %u\n", byte_stream_size);

	return error_code::NONE;
}

error_code personal_scramble_decode(utils::pixels::types pixel_type,
									uint32_t height,
									uint32_t width,
									uint8_t*& image_data,
									uint32_t& secret_byte_stream_size,
									uint8_t*& secret_byte_stream,
									std::string password) 
{
	TRY(decode_metadata(pixel_type, width, image_data, secret_byte_stream_size));

	secret_byte_stream = new uint8_t[secret_byte_stream_size];
	uint8_t secret_byte = 0;
	uint8_t current_bit_index = 0;
	uint32_t secret_data_index = 0;
	uint8_t pixel_size = static_cast<uint8_t>(pixel_type); 
	size_t message_hash_used_as_seed = std::hash<std::string>{}(password);

	for (uint32_t line_index = 0; line_index < height; line_index += 8) {
		for (uint32_t col_index = 0; col_index < width; col_index += 8) {
			if (line_index == 0 && col_index == 0)
				continue; //ignore the first 8x8 pixel block of the image, because thats the metadata block

			uint8_t block_width = (width - col_index) > 8 ? 8 : (width - col_index);
			uint8_t block_height = (height - line_index) > 8 ? 8 : (height - line_index);
			auto order = get_permutation_order(block_width * block_height, message_hash_used_as_seed); //maybe change the hash each time?

			for (auto relative_inner_pixel_index : order) {
				uint32_t absolute_pixel_line = line_index + relative_inner_pixel_index / block_width;
				uint32_t absolute_pixel_column = col_index + relative_inner_pixel_index % block_width;

				for (uint8_t pixel_byte_index = 0; pixel_byte_index < pixel_size; pixel_byte_index++) {
					uint8_t* image_data_byte = &image_data[pixel_size * width * absolute_pixel_line + pixel_size * absolute_pixel_column + pixel_byte_index];

					secret_byte |= ((*image_data_byte) % 2) << current_bit_index;
					current_bit_index++;

					if (current_bit_index == 8) {
						secret_byte_stream[secret_data_index++] = secret_byte;
						current_bit_index = 0;
						secret_byte = 0;


						if (secret_data_index == secret_byte_stream_size)
							return error_code::NONE;
					}
				}
			}
		}
	}


	return error_code::COVER_FILE_ERROR;
}



std::vector<uint8_t> get_permutation_order(uint8_t size, size_t seed_from_hash) {
	std::vector<uint8_t> pixel_order;

	for (uint8_t index = 0; index < size; index++)
		pixel_order.push_back(index);

	std::srand(seed_from_hash);
	std::shuffle(pixel_order.begin(), pixel_order.end(), std::default_random_engine(seed_from_hash));

	return pixel_order;
}
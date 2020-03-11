#include <general/image_algorithms.h>

error_code embed_metadata(utils::pixels::types pixel_type, uint32_t width, uint8_t*& image_data, uint32_t byte_stream_size) {
	if (pixel_type == utils::pixels::types::RGB || pixel_type == utils::pixels::types::BGR) {
		//working on 8x8 grids, when we typed the first 24/32 bits we will go and type the remaining 8 bits on the second line
		for (short bit_index = 0; bit_index < 32; bit_index++) {
			uint8_t& byte_to_be_rewritten = image_data[width * (bit_index / 24) + bit_index % 24];
			printf("[RGB|BGR]Rewritting byte %u from %02x ", (uint32_t)(width * (bit_index / 24) + bit_index % 24), byte_to_be_rewritten);

			utils::setLSB(byte_to_be_rewritten, (byte_stream_size >> bit_index) & 1);
			printf("to %02x\n", byte_to_be_rewritten);
		}
	}
	else {
		for (short bit_index = 0; bit_index < 32; bit_index++) {
			uint8_t& byte_to_be_rewritten = image_data[bit_index];
			printf("[RGBA]Rewriting byte %u from %02x ", (uint32_t)(bit_index), byte_to_be_rewritten);

			utils::setLSB(byte_to_be_rewritten, (byte_stream_size >> bit_index) & 1);
			printf("to %02x\n", byte_to_be_rewritten);
		}
	}

	return error_code::NONE;
}

error_code personal_scramble_embed(utils::pixels::types pixel_type,
									uint32_t height,
									uint32_t width,
									uint8_t*& image_data,
									uint32_t byte_stream_size,
									uint8_t*& byte_stream) 
{
	//we know the first 32 bytes contain the 4 bytes that determine the embedded file length
	//for those 32 bytes 10 pixels and 2 color channels from 11th pixel are needed(if pixel type is RGB)
	//for those 32 bytes 8 pixels are needed(if pixel type is RGBA)
	TRY(embed_metadata(pixel_type, width, image_data, byte_stream_size));

	//effectively embedding the data
	//IGNORE THE FIRST 4 BYTES OF BYTE_STREAM(THEY ARE THE BYTE_STREAM LENGTH)
	for (uint32_t line_index = 0; line_index < width; line_index += 8) {
		for (uint32_t col_index = 0; col_index < height; col_index += 8) {
			break;
		}
	}

	return error_code::NONE;
}




//decode functions for the personal scramble algorithm

error_code decode_metadata(utils::pixels::types pixel_type, uint32_t width, uint8_t*& image_data, uint32_t& byte_stream_size) {
	byte_stream_size = 0;

	if (pixel_type == utils::pixels::types::RGB || pixel_type == utils::pixels::types::BGR) {
		//working on 8x8 grids, when we typed the first 24/32 bits we will go and type the remaining 8 bits on the second line
		for (short bit_index = 0; bit_index < 32; bit_index++) {
			uint8_t& source_byte = image_data[width * (bit_index / 24) + bit_index % 24];
			byte_stream_size |= (source_byte % 2) << bit_index;
		}
	}
	else {
		for (short bit_index = 0; bit_index < 32; bit_index++) {
			uint8_t& source_byte = image_data[bit_index];
			byte_stream_size |= (source_byte % 2) << bit_index;
		}
	}

	printf("[PersonalScramble]Secret stream size is %u\n", byte_stream_size);

	return error_code::NONE;
}

error_code personal_scramble_decode(utils::pixels::types pixel_type,
									uint32_t height,
									uint32_t width,
									uint8_t*& image_data,
									uint32_t& byte_stream_size,
									uint8_t*& byte_stream) 
{
	TRY(decode_metadata(pixel_type, width, image_data, byte_stream_size));

	return error_code::NONE;
}
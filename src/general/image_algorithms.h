
#include "utils.h"

using BGRPixel = utils::pixels::BGRPixel;
using RGBPixel = utils::pixels::RGBPixel;


//sequential encoding and decoding function prototypes for an image data byte stream
error_code simple_sequential_embed(uint64_t image_data_size,
	uint8_t*& image_data,
	uint32_t byte_stream_size,
	uint8_t*& byte_stream
);

error_code simple_sequential_decode(uint64_t image_data_size,
	uint8_t*& image_data,
	uint32_t& secret_data_size,
	uint8_t*& secret_data_stream
);
//personal scramble embeds for all pixels

error_code personal_scramble_embed(uint64_t image_data_size,
	uint8_t*& image_data,
	uint32_t byte_stream_size,
	uint8_t*& byte_stream);
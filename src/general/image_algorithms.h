
#include "utils.h"

using BGRPixel = utils::pixels::BGRPixel;
using RGBPixel = utils::pixels::RGBPixel;


//sequential embeds for different pixels
error_code simple_sequential_embed(uint32_t image_width,
	uint32_t image_height,
	RGBPixel**& image_data,
	uint32_t byte_stream_size,
	uint8_t*& byte_stream);

error_code simple_sequential_embed(uint32_t image_width,
	uint32_t image_height,
	BGRPixel**& image_data,
	uint32_t byte_stream_size,
	uint8_t*& byte_stream);

//personal scramble embeds for all pixels

error_code personal_scramble_embed(uint32_t image_width,
	uint32_t image_height,
	BGRPixel**& image_data,
	uint32_t byte_stream_size,
	uint8_t*& byte_stream);
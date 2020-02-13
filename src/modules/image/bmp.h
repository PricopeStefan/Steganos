#pragma once

#include "general/module.h"


namespace {
	//structure format of a bmp file taken from(only the metadata part)
	//http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
	#pragma pack(push, 1)
	typedef struct BMPMetaStruct {
		//generic file header
		uint16_t signature;
		uint32_t file_size;
		uint32_t reserved;
		uint32_t data_offset;
		//image info header
		uint32_t size;
		uint32_t width;
		uint32_t height;
		uint16_t planes;
		uint16_t bits_per_pixel;
		uint32_t compression;
		uint32_t image_size;
		uint32_t x_pixels_per_meter;
		uint32_t y_pixels_per_meter;
		uint32_t colors_used;
		uint32_t important_colors;
	};
	#pragma pack(pop)
}

#include <iostream>
class BMPModule : public Module {
private:
	//structure responsible for holding the bmp metadata
	BMPMetaStruct cover_image_metadata;
public:
	BMPModule(const char* cover_file_path, const char* secret_file_path) {
		TRY(load_cover(cover_file_path));
		TRY(load_secret(secret_file_path));

		cover_stream->read((char*)&cover_image_metadata, sizeof(BMPMetaStruct));
	}

	inline const uint32_t get_cover_width() const {
		return cover_image_metadata.width;
	}

	inline const uint32_t get_cover_height() const {
		return cover_image_metadata.height;
	}
};
#pragma once

#include "general/module.h"
#include "general/utils.h"

enum BMPModuleSupportedAlgorithms {
	//the classic one = the last bit of each pixel is changed to represent a bit of the data
	SEQUENTIAL, 
	//divide the image data matrix in 8x8 submatrixes and randomize using a given password 
	//the order of the writing of the secret data - generate a random permutation containing numbers 1-64
	//and write the data bits following that permutation
	PERSONAL_SCRAMBLE, 
	//TO DO : add more algorithms
};


//use a map instead of a struct?
struct BMPModuleOptions {
	bool compress_secret = false; //bool indicating whether to compress the secret data or not
	BMPModuleSupportedAlgorithms algorithm = BMPModuleSupportedAlgorithms::SEQUENTIAL;
};

namespace {
	//structure format of a bmp file taken from(only the metadata part)
	//http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
	#pragma pack(push, 1)
	struct BMPMetaStruct {
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
	
	//BMP images store pixel information backwards(no idea why, should research)
	struct BGRPixel {
		uint8_t blue = 255;
		uint8_t green = 255;
		uint8_t red = 255;
	};
}


#include <iostream>
class BMPModule : public Module {
private:
	//structure responsible for holding the bmp metadata
	BMPMetaStruct cover_image_metadata;
	//2d array of the raw pixels of the cover image
	BGRPixel** cover_image_data = nullptr;
	//pointer to the raw bytes of the secret data
	uint8_t* secret_data = nullptr;

	void read_cover_info() {
		cover_stream->read((char*)&cover_image_metadata, sizeof(BMPMetaStruct));

		std::cout << "BMP Image data is found beginning at " << cover_image_metadata.data_offset << std::endl;
		if (sizeof(BMPMetaStruct) > cover_image_metadata.data_offset) {
			std::cout << "BMP file is invalid(broken data offset header)" << std::endl;
			exit(-1);
		}
		

		size_t bytes_until_data = cover_image_metadata.data_offset - sizeof(BMPMetaStruct);
		std::cout << "Ignoring " << bytes_until_data << " bytes to get to the actual image data" << std::endl;
		//really probably shouldn't do this and better research the bmp subvariants and their specific headers
		cover_stream->ignore(bytes_until_data);

		uint32_t padded_width = cover_image_metadata.width;
		if (cover_image_metadata.width % 4 != 0)
			padded_width += (4 - cover_image_metadata.width % 4);

		//alocating space for the rows of pixels
		cover_image_data = new BGRPixel* [cover_image_metadata.height];
		
		//BMP images store data bottom-up : the bottom line is stored as the first in the byte stream
		for (int32_t line_index = cover_image_metadata.height; line_index >= 0; line_index--) {
			//a line of pixels is called a scan line in a BMP file
			BGRPixel* scan_line_pixels = new BGRPixel[padded_width];
			cover_stream->read(reinterpret_cast<char*>(scan_line_pixels), padded_width * sizeof(BGRPixel));
			
			cover_image_data[line_index] = scan_line_pixels;
		}
	}

	int32_t sequential_handler(const BMPModuleOptions& steg_options) {

		return 0;
	}

	int32_t personal_scramble_handler(const BMPModuleOptions& steg_options) {
		
		return 0;
	}

public:
	BMPModule(const char* cover_file_path, const char* secret_file_path) {
		TRY(load_cover(cover_file_path));
		TRY(load_secret(secret_file_path));

		read_cover_info();
	}
	BMPModule(const char* cover_file_path, const char* secret_file_path, const BMPModuleOptions& steg_options) {
		BMPModule(cover_file_path, secret_file_path);

		launch_steganos(steg_options);
	}

	inline const uint32_t get_cover_width() const {
		return cover_image_metadata.width;
	}
	inline const uint32_t get_cover_height() const {
		return cover_image_metadata.height;
	}

	int32_t launch_steganos(const BMPModuleOptions& steg_options) {
		if (steg_options.compress_secret)
			utils::compress_data(secret_data);

		switch (steg_options.algorithm)
		{
			case BMPModuleSupportedAlgorithms::SEQUENTIAL:
				return sequential_handler(steg_options);
			case BMPModuleSupportedAlgorithms::PERSONAL_SCRAMBLE:
				return personal_scramble_handler(steg_options);
			default:
				std::cout << "I have no idea what u want me to do, this shouldnt happen(oh yes, invalid algorithm given, but how?)\n";
				break;
		}

		return -1;
	}

	~BMPModule() {
		//deleting the allocated memory for each line of pixels of the cover image
		for (uint32_t line_index = 0; line_index < cover_image_metadata.height; line_index++) {
			delete[] cover_image_data[line_index];
		}

		//i should also do this but it breaks?
		//delete[] cover_image_data;
	}
};
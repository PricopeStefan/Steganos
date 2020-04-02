#pragma once

#include "general/utils.h"

//supported algorithms for the bmp files
#include <general/image_algorithms.h>

using BGRPixel = utils::pixels::BGRPixel;

enum class BMPModuleSupportedAlgorithms {
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
	bool encrypt_secret = false;
	std::string password = "fasf";

	bool compress_secret = false; //bool indicating whether to compress the secret data or not
	BMPModuleSupportedAlgorithms algorithm = BMPModuleSupportedAlgorithms::SEQUENTIAL;
};

//structure format of a bmp file taken from(only the metadata part)
//http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
//need those pragma pack/push because the stack doesn't like not being aligned(cant place 2 bytes, then 4 bytes, its not neat)
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

class BMPModule {
protected:
	std::ifstream* bmp_stream = nullptr;
	//structure responsible for holding the bmp metadata
	BMPMetaStruct cover_image_metadata;
	//2d array of the raw pixels of the cover image
	//BGRPixel** cover_image_data = nullptr;
	uint8_t* cover_image_data = nullptr;

	error_code read_cover_metadata();
	error_code read_cover_data();
public:
	BMPModule(const char* bmp_file_path);
	~BMPModule();

	const BMPMetaStruct& get_metadata() const;
	uint32_t get_padded_width() const {
		if (cover_image_metadata.width % 4 == 0)
			return cover_image_metadata.width;
		
		return cover_image_metadata.width + (4 - cover_image_metadata.width % 4);
	}

	error_code write_bmp(const char* output_path = "output.bmp");
};

class BMPEncoderModule : public BMPModule {
private:
	std::ifstream* secret_stream = nullptr;
	//pointer to the raw bytes of the secret data
	uint8_t* secret_data = nullptr;
	uint32_t secret_data_size = 0;

	error_code simple_sequential_embed_handler(const BMPModuleOptions& steg_options);
	error_code personal_scramble_embed_handler(const BMPModuleOptions& steg_options);

public:
	BMPEncoderModule(const char* cover_file_path);
	BMPEncoderModule(const char* cover_file_path, const char* secret_file_path);
	BMPEncoderModule(const char* cover_file_path, const char* secret_file_path, const BMPModuleOptions& steg_options);
	
	~BMPEncoderModule();
	
	error_code load_secret(const char* secret_file_path);

	error_code launch_steganos(const BMPModuleOptions& steg_options = BMPModuleOptions());
};

class BMPDecoderModule : public BMPModule {
private:
	//pointer to the secret data
	uint8_t* secret_data = nullptr;
	//number of bytes that the secret data holds
	uint32_t secret_data_size = 0;

	error_code write_secret(const char* output_path = "hidden.bin") const;

	error_code sequential_handler(const BMPModuleOptions& steg_options);
	error_code personal_scramble_handler(const BMPModuleOptions& steg_options);

public:
	BMPDecoderModule(const char* embedded_path);
	BMPDecoderModule(const char* embedded_path, const BMPModuleOptions steg_options);
	~BMPDecoderModule();

	error_code launch_steganos(const BMPModuleOptions& steg_options = BMPModuleOptions());
};
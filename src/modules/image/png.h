#pragma once
#include "general/utils.h"
//supported algorithms for the bmp files
#include <general/image_algorithms.h>

#include <vector>

using RGBPixel = utils::pixels::RGBPixel;
using RGBAPixel = utils::pixels::RGBAPixel;

#define PNG_HEADER "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A"

enum class PNGModuleSupportedAlgorithms {
	//the classic one = the last bit of each pixel is changed to represent a bit of the data
	SEQUENTIAL,
	//divide the image data matrix in 8x8 submatrixes and randomize using a given password 
	//the order of the writing of the secret data - generate a random permutation containing numbers 1-64
	//and write the data bits following that permutation
	PERSONAL_SCRAMBLE,
	//TO DO : add more algorithms(extract only red, blue or green channels)
};

struct PNGModuleOptions {
	bool encrypt_secret = false;
	std::string password = "png_password";

	bool compress_secret = false; //bool indicating whether to compress the secret data or not
	PNGModuleSupportedAlgorithms algorithm = PNGModuleSupportedAlgorithms::SEQUENTIAL;
};

//format for the png chunks, based on the original specification from http://www.libpng.org/pub/png/spec/1.2/PNG-Structure.html
//composition of a png file can be seen in the theoretical paper or https://en.wikipedia.org/wiki/Portable_Network_Graphics
//also very imoprtant - PNG is in big endian byte order
#pragma pack(push, 1)
struct PNGChunkStruct {
	uint32_t length = 0;
	uint32_t type = 0;
	uint8_t* data = nullptr;
	uint32_t crc = 0;

	PNGChunkStruct(uint32_t length, uint32_t type, uint8_t* data, uint32_t crc) 
		: length{ length }, type{ type }, data{ data }, crc{ crc } {}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PNGMetadataStruct {
	uint32_t width = 0;
	uint32_t height = 0;
	uint8_t bit_depth = 0;
	uint8_t color_type = 0;
	uint8_t compression_method = 0;
	uint8_t filter_method = 0;
	uint8_t interlace_method = 0;
};
#pragma pack(pop)


class PNGModule {
protected:
	std::ifstream * png_stream = nullptr;
	std::vector<PNGChunkStruct> png_chunks;

	error_code read_cover_data();

	void convert_chunk_type_to_string(char(&buf)[4], uint32_t uval) const;

public:
	PNGModule(const char* png_file_path);
	~PNGModule();

	const PNGMetadataStruct get_metadata() const;


	error_code write_png(const char* output_path = "output.png");
};



class PNGEncoderModule : public PNGModule {
private:
	std::ifstream* secret_stream = nullptr;
	//pointer to the raw bytes of the secret data
	uint8_t* secret_data = nullptr;
	uint32_t secret_data_size = 0;

	error_code simple_sequential_embed_handler(const PNGModuleOptions& steg_options);
	error_code personal_scramble_embed_handler(const PNGModuleOptions& steg_options);


	uint32_t crc_table[256];
	void make_crc_table();
	uint32_t update_crc(uint32_t crc, uint8_t* buf, int32_t len);
	uint32_t crc(uint8_t* buf, int32_t len);
public:
	PNGEncoderModule(const char* cover_file_path);
	PNGEncoderModule(const char* cover_file_path, const char* secret_file_path);
	PNGEncoderModule(const char* cover_file_path, const char* secret_file_path, const PNGModuleOptions& steg_options);

	~PNGEncoderModule();

	error_code load_secret(const char* secret_file_path);

	error_code launch_steganos(const PNGModuleOptions& steg_options = PNGModuleOptions());
};
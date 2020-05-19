#pragma once

#include <fstream>
#include <iostream>



enum class error_code {
	MISC_ERROR = -1,
	NONE = 0,
	STREAM_ERROR = 1,
	COVER_FILE_ERROR = 2,
	SECRET_FILE_ERROR = 3,
	DATA_ERROR = 4,
	INSUFFICIENT_COVER_SPACE = 5
};

#define TRY(X) cool_assert((X), __FILE__, __LINE__);
inline void cool_assert(error_code code, const char* file, int line) {
	if (code != error_code::NONE) {
		printf("[Assert error]Message = %d in file %s at line %d\n", code, file, line);
		exit(static_cast<int>(code));
	}
}



namespace utils {
	error_code load_stream(const char* file_path, std::ifstream*& stream, std::ios_base::openmode stream_options = std::ios_base::binary);
	
	//given a pointer to a stream opened in binary mode and a pointer for a byte array,
	//reads the byte array and returns how many bytes were read
	error_code read_byte_stream(std::ifstream* stream, uint8_t*& byte_stream, uint32_t& stream_size);

	int32_t compress_data(uint8_t*& data_bytes);

	void setLSB(uint8_t& value, const bool& bit);
	void setLSB(uint16_t& value, const bool& bit);
	void setLSB(uint32_t& value, const bool& bit);

	uint8_t read_byte_from_lsbs(uint8_t* byte_stream, uint64_t byte_stream_size);

	uint32_t convert_synchsafe_uint_to_normal_uint(uint32_t synch_safe);

	namespace pixels {
		//pixels types used in the internal storage of the image data of different file formats
		enum class types {
			//BGR is the type of pixel used by the BMP file format - 24 bits/pixel
			BGR = 3,
			//RGB is the normal type of pixel used by JPEG, PNG etc. - 24 bits/pixel
			RGB = 3,
			//RGBA is common in PNG file format, 3 normal color channels + alpha/opacity channel - 32 bits/pixel
			RGBA = 4
		};


		//BMP images store pixel information backwards(no idea why, should research)
		struct BGRPixel {
			uint8_t blue = 0;
			uint8_t green = 0;
			uint8_t red = 0;
		};

		struct RGBPixel {
			uint8_t red = 0;
			uint8_t green = 0;
			uint8_t blue = 0;
		};

		struct RGBAPixel {
			uint8_t red = 0;
			uint8_t green = 0;
			uint8_t blue = 0;
			uint8_t alpha = 0;
		};
	}
}

#pragma once
#include <fstream>

#include "general/utils.h"

class Module {
protected:
	std::ifstream* cover_stream = nullptr;
	uint8_t* cover_data = nullptr;
	std::string cover_file_path;


	virtual error_code read_cover_data() = 0;
	virtual error_code write_cover() = 0;
};

class EncoderModule : public Module {
protected:
	std::ifstream* secret_stream = nullptr;
	uint8_t* secret_data = nullptr;
	uint32_t secret_data_size = 0;
	std::string secret_file_path;

public:
	virtual error_code launch_steganos() = 0;
};

class DecoderModule : public Module {
protected:
	//pointer to the secret data
	uint8_t* secret_data = nullptr;
	//number of bytes that the secret data holds
	uint32_t secret_data_size = 0;

	virtual error_code write_secret(const char* output_path = "hidden.bin") const = 0;
public:
	virtual error_code launch_steganos() = 0;
};
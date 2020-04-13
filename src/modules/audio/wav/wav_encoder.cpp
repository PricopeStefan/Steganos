#include <modules/audio/wav.h>

WAVEncoderModule::WAVEncoderModule(const char* cover_file_path) : WAVModule(cover_file_path) {
	//do nothing if there is no secret provided, just load the BMP into memory
	printf("Created new WAVEncoderModule\n");
}
WAVEncoderModule::WAVEncoderModule(const char* cover_file_path, const char* secret_file_path) : WAVModule(cover_file_path) {
	TRY(utils::load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));
}
WAVEncoderModule::WAVEncoderModule(const char* cover_file_path, const char* secret_file_path, const WAVModuleOptions& steg_options) : WAVModule(cover_file_path) {
	TRY(utils::load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));

	launch_steganos(steg_options);
}

WAVEncoderModule::~WAVEncoderModule() {
	//delete the secret data allocated memory
	if (secret_data != nullptr)
		delete[] secret_data;

	if (secret_stream != nullptr)
		delete secret_stream;
}

error_code WAVEncoderModule::simple_sequential_embed_handler(const WAVModuleOptions& steg_options) {
	uint64_t successful_written_bytes = 0;

	uint32_t secret_data_byte_index = 0;
	uint8_t bit_index = 0;
	uint8_t current_byte = secret_data[secret_data_byte_index];
	//printf("Current byte = %02X\n", current_byte);

	//helper lambda function to advance to the next data bit from the secret stream
	//returns how many bytes there are left to write
	auto advance_to_next_bit = [&]() {
		//go to the next bit
		bit_index++;
		current_byte >>= 1;

		if (bit_index == 8) {
			secret_data_byte_index++;
			successful_written_bytes++;
			//check to see if done with the writing of the secret data
			if (secret_data_byte_index == secret_data_size)
				return 0u; //no bits left to write, return 0

			bit_index = 0;
			current_byte = secret_data[secret_data_byte_index];
			//printf("Current byte = %02X\n", current_byte);
		}

		//there are still byte_stream_size - secret_data_byte_index bytes left to write
		return (secret_data_size - secret_data_byte_index);
	};

	for (uint32_t current_byte_index = 1; current_byte_index < cover_data_length; current_byte_index += 256) {
		uint8_t& image_data_byte = cover_data[current_byte_index];

		utils::setLSB(image_data_byte, current_byte & 1);

		if (advance_to_next_bit() == 0)
			return error_code::NONE;
	}

	if (successful_written_bytes == 0) {
		printf("There was an error while writing to the file\n");
		return error_code::COVER_FILE_ERROR;
	}

	return error_code::NONE;
};


error_code WAVEncoderModule::load_secret(const char* secret_file_path) {
	TRY(utils::load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));

	return error_code::NONE;
}

error_code WAVEncoderModule::launch_steganos(const WAVModuleOptions& steg_options) {
	if (steg_options.compress_secret)
		utils::compress_data(secret_data);

	switch (steg_options.algorithm)
	{
	case WAVModuleSupportedAlgorithms::SEQUENTIAL:
		TRY(simple_sequential_embed_handler(steg_options));
		break;
	default:
		std::cout << "Algorithm not yet implemented, its on my TO DO\n";
		return error_code::MISC_ERROR;
	}


	TRY(write_wav());

	return error_code::NONE;
}
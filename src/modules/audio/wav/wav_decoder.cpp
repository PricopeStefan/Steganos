#include <modules/audio/wav.h>


error_code WAVDecoderModule::write_secret(const char* output_path) const {
	if (secret_data_size == 0 || secret_data == nullptr) {
		return error_code::DATA_ERROR;
	}

	std::ofstream embedded_stream(output_path, std::ios_base::binary);
	if (!embedded_stream.is_open() || embedded_stream.fail()) {
		return error_code::STREAM_ERROR;
	}

	//TO DO : check if writes succedded
	embedded_stream.write(reinterpret_cast<char*>(secret_data), secret_data_size - 4); // - 4 because we added 4 bytes for the stream length
	embedded_stream.close();

	return error_code::NONE;
}

error_code WAVDecoderModule::parse_secret_metadata(const WAVModuleOptions& steg_options, uint32_t& actual_data_starting_offset) {
	secret_data_size = 0;
	uint8_t current_bit_index = 0;
	//this is the last possible starting byte offset while also taking in considedration the number of channels
//and the total bits per sample
	uint32_t last_starting_offset = cover_data_length - (cover_metadata.bits_per_sample / 8) * cover_metadata.num_channels;
	//how many bytes to skip with each loop based on the parameter given to the module, the bits per sample and the number of channels
	uint32_t bytes_to_skip = steg_options.number_of_samples_to_skip * (cover_metadata.bits_per_sample / 8) * cover_metadata.num_channels;

	for (uint32_t current_byte_index = 1; current_byte_index < last_starting_offset; current_byte_index += bytes_to_skip) {
		for (uint16_t channel_index = 0; channel_index < cover_metadata.num_channels; channel_index++) {
			uint8_t& audio_data_byte = cover_data[current_byte_index + (cover_metadata.bits_per_sample / 8) * channel_index];

			secret_data_size |= ((audio_data_byte) % 2) << current_bit_index;
			current_bit_index++;

			if (current_bit_index == 32) {
				if (channel_index != cover_metadata.num_channels - 1)
					actual_data_starting_offset = current_byte_index + (cover_metadata.bits_per_sample / 8) * (channel_index + 1);
				else
					actual_data_starting_offset = current_byte_index + bytes_to_skip;
				return error_code::NONE;
			}
		}
	}

	return error_code::COVER_FILE_ERROR;
}

error_code WAVDecoderModule::sequential_handler(const WAVModuleOptions& steg_options) {
	uint32_t actual_data_starting_offset = 0;
	TRY(this->parse_secret_metadata(steg_options, actual_data_starting_offset));
	printf("Secret data size in decoder = %u\n", secret_data_size);
	printf("actual secret data starts at %u\n", actual_data_starting_offset);

	uint32_t secret_data_byte_index = 0;
	uint8_t bit_index = 0;
	secret_data = new uint8_t[secret_data_size];
	uint8_t secret_byte = 0;
	uint8_t current_bit_index = 0;


	//this is the last possible starting byte offset while also taking in considedration the number of channels
	//and the total bits per sample
	uint32_t last_starting_offset = cover_data_length - (cover_metadata.bits_per_sample / 8) * cover_metadata.num_channels;
	//how many bytes to skip with each loop based on the parameter given to the module, the bits per sample and the number of channels
	uint32_t bytes_to_skip = steg_options.number_of_samples_to_skip * (cover_metadata.bits_per_sample / 8) * cover_metadata.num_channels;

	for (uint32_t current_byte_index = actual_data_starting_offset; current_byte_index < last_starting_offset; current_byte_index += bytes_to_skip) {
		for (uint16_t channel_index = 0; channel_index < cover_metadata.num_channels; channel_index++) {
			uint8_t& audio_data_byte = cover_data[current_byte_index + (cover_metadata.bits_per_sample / 8) * channel_index];

			secret_byte |= ((audio_data_byte) % 2) << current_bit_index;
			current_bit_index++;

			if (current_bit_index == 8) {
				secret_data[secret_data_byte_index++] = secret_byte;
				current_bit_index = 0;
				secret_byte = 0;


				if (secret_data_byte_index == secret_data_size)
					return error_code::NONE;
			}
		}
	}

	
	return error_code::NONE;
}

WAVDecoderModule::WAVDecoderModule(const char* embedded_path) : WAVModule(embedded_path) {
	//do nothing if no options are given, just load the bmp into memory
}
WAVDecoderModule::WAVDecoderModule(const char* embedded_path, const WAVModuleOptions steg_options) : WAVModule(embedded_path) {
	TRY(launch_steganos(steg_options));
}

WAVDecoderModule::~WAVDecoderModule() {
	delete[] secret_data;
}

error_code WAVDecoderModule::launch_steganos(const WAVModuleOptions& steg_options) {
	switch (steg_options.algorithm)
	{
	case WAVModuleSupportedAlgorithms::SEQUENTIAL:
		TRY(sequential_handler(steg_options));
		write_secret(steg_options.output_path.c_str());
		break;
	default:
		std::cout << "Algorithm not yet implemented, its on my TO DO\n";
		return error_code::MISC_ERROR;
	}
}
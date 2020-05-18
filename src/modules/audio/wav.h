#pragma once

#include <fstream>
#include <vector>
#include <general/utils.h>
#include <general/image_algorithms.h>

//section containing the specific headers ids of wav files as int values
//found at http://soundfile.sapp.org/doc/WaveFormat/
#define RIFF_HEADER_ID 1179011410
#define FMT_HEADER_ID 544501094
#define DATA_HEADER_ID 1635017060

enum class WAVModuleSupportedAlgorithms {
	//the classic one = the last bit of each sample is changed to represent a bit of the data
	SEQUENTIAL,
	//embed the message into the metadata of the cover file
	METADATA,
	//embed after the audio data has ended
	AFTER_END
};

struct WAVModuleOptions {
	bool encrypt_secret = false;
	std::string password = "fasf";
	uint32_t number_of_samples_to_skip = 192; //proven to be the best default by testing

	bool compress_secret = false; //bool indicating whether to compress the secret data or not
	WAVModuleSupportedAlgorithms algorithm = WAVModuleSupportedAlgorithms::SEQUENTIAL;
};

#pragma pack(push, 1)
struct WAVMetaStruct {
	uint32_t chunk_id;
	uint32_t chunk_size;
	uint32_t format;
	uint32_t subchunk_1_id;
	uint32_t subchunk_1_size;
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
};
#pragma pack(pop)

class WAVModule {
protected:
	std::ifstream* wav_stream = nullptr;
	//structure responsible for holding the wav metadata
	WAVMetaStruct cover_metadata = WAVMetaStruct();
	std::vector<uint8_t> additional_cover_metadata;
	uint32_t cover_data_length = 0;
	uint8_t* cover_data = nullptr;

	error_code read_cover_metadata();
	error_code read_cover_data();
public:
	WAVModule(const char* wav_file_path);
	~WAVModule();

	const WAVMetaStruct& get_metadata() const;

	error_code write_wav(const char* output_path = "output.wav");
};


class WAVEncoderModule : public WAVModule {
private:
	std::ifstream* secret_stream = nullptr;
	//pointer to the raw bytes of the secret data
	uint8_t* secret_data = nullptr;
	uint32_t secret_data_size = 0;

	error_code simple_sequential_embed_handler(const WAVModuleOptions& steg_options);
public:
	WAVEncoderModule(const char* cover_file_path);
	WAVEncoderModule(const char* cover_file_path, const char* secret_file_path);
	WAVEncoderModule(const char* cover_file_path, const char* secret_file_path, const WAVModuleOptions& steg_options);

	~WAVEncoderModule();

	error_code load_secret(const char* secret_file_path);

	error_code launch_steganos(const WAVModuleOptions& steg_options = WAVModuleOptions());
};

class WAVDecoderModule : public WAVModule {
private:
	//pointer to the secret data
	uint8_t* secret_data = nullptr;
	//number of bytes that the secret data holds
	uint32_t secret_data_size = 0;

	error_code write_secret(const char* output_path = "hidden.bin") const;

	error_code parse_secret_metadata(const WAVModuleOptions& steg_options, uint32_t& actual_data_starting_offset);
	error_code sequential_handler(const WAVModuleOptions& steg_options);

public:
	WAVDecoderModule(const char* embedded_path);
	WAVDecoderModule(const char* embedded_path, const WAVModuleOptions steg_options);
	~WAVDecoderModule();

	error_code launch_steganos(const WAVModuleOptions& steg_options = WAVModuleOptions());
};
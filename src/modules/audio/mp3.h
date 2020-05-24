#pragma once

#include <fstream>
#include <vector>
#include <general/utils.h>



enum class MP3ModuleSupportedAlgorithms {
	//the classic one = the last bit of each sample is changed to represent a bit of the data
	SEQUENTIAL,
	//embed the message into the metadata of the cover file
	METADATA,
	//embed after the audio data has ended
	AFTER_END
};

struct MP3ModuleOptions {
	bool encrypt_secret = false;
	std::string password = "fasf";
	uint32_t number_of_samples_to_skip = 48; //proven to be the best default by testing

	bool compress_secret = false; //bool indicating whether to compress the secret data or not
	MP3ModuleSupportedAlgorithms algorithm = MP3ModuleSupportedAlgorithms::SEQUENTIAL;
};

//based on https://id3.org/id3v2-00
#pragma pack(push, 1)
struct MP3MetaStruct { //aka ID3 header
	uint8_t identifier[3];
	uint8_t major_version;
	uint8_t minor_version;
	uint8_t flags_byte;
	uint32_t header_size;
};
#pragma pack(pop)



//everything that is not a bit is going to be uint8_t because we cant work bit by bit
//we'll have helper functions that read a uint32_t and converts it to this header and back
//based on http://www.mpgedit.org/mpgedit/mpeg_format/mpeghdr.htm
struct MP3AudioFrameHeader {
	uint16_t sync_word;
	uint8_t version;
	uint8_t layer;
	uint8_t error_protection;
	uint8_t bit_rate;
	uint8_t frequency;
	uint8_t padding_bit;
	uint8_t private_bit;
	uint8_t mode;
	uint8_t mode_extension; //dont really care
	uint8_t copyrighted;
	uint8_t is_original;
	uint8_t emphasis;
};

class MP3Module {
protected:
	std::ifstream* mp3_stream = nullptr;
	//structure responsible for holding the ID3 mp3 metadata
	MP3MetaStruct cover_metadata;
	std::vector<ID3v3Frame> metadata_frames;
	uint32_t cover_data_length = 0;
	uint8_t* cover_data = nullptr;

	MP3AudioFrameHeader convert_binary_to_frame_header(uint32_t frame_header_binary);
	uint32_t convert_frame_header_to_binary(MP3AudioFrameHeader frame_header_structure);
	uint32_t get_frame_size(MP3AudioFrameHeader& frame_header);
	uint32_t get_frame_bitrate(MP3AudioFrameHeader& frame_header);
	uint32_t get_frame_frequency(MP3AudioFrameHeader& frame_header);

	error_code read_cover_data();
public:
	MP3Module(const char* mp3_file_path);
	~MP3Module();

	//const WAVMetaStruct& get_metadata() const;

	error_code write_cover(const char* output_path = "output.mp3");
};



class MP3EncoderModule : public MP3Module {
private:
	std::ifstream* secret_stream = nullptr;
	uint8_t* secret_data = nullptr;
	uint32_t secret_data_size = 0;

	error_code image_embed(const MP3ModuleOptions& steg_options);

	error_code splice_apic_frame(ID3v3APICFrameData& apic_custom_frame, ID3v3Frame& custom_frame);
public:
	MP3EncoderModule(const char* cover_file_path);
	MP3EncoderModule(const char* cover_file_path, const char* secret_file_path);
	~MP3EncoderModule();

	error_code launch_steganos(const MP3ModuleOptions& steg_options = MP3ModuleOptions());
};
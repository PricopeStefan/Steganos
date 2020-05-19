#include <modules/audio/mp3.h>
#include "..\mp3.h"

MP3Module::MP3Module(const char* mp3_file_path) {
	TRY(utils::load_stream(mp3_file_path, mp3_stream));

	TRY(read_cover_data());
}
MP3Module::~MP3Module() {
	//deleting the allocated memory for the samples of the cover track
	delete[] cover_data;

	if (mp3_stream != nullptr)
		delete mp3_stream;

	std::cout << "Deleted audio data and its associated stream" << std::endl;
}

error_code MP3Module::read_cover_data() {
	uint32_t actual_data_offset = 0;

	mp3_stream->read((char*)&cover_metadata, sizeof(MP3MetaStruct));
	if (cover_metadata.identifier[0] != 'I' && cover_metadata.identifier[1] != 'D' && cover_metadata.identifier[2] != '3')
		return error_code::COVER_FILE_ERROR;

	printf("Read an MP3, ID3 v2.%d.%d\n", cover_metadata.major_version, cover_metadata.minor_version);
	printf("Total IDv3 Header Size = %u\n", utils::convert_synchsafe_uint_to_normal_uint(cover_metadata.header_size));
	actual_data_offset += utils::convert_synchsafe_uint_to_normal_uint(cover_metadata.header_size);
	bool is_footer_present = (cover_metadata.flags_byte >> 4) % 2;
	if (is_footer_present)
		actual_data_offset += 10;

	printf("Actual MP3 data starting from %u\n", actual_data_offset);
	mp3_stream->ignore(actual_data_offset); // get to the starting bytes of the thingie
	 
	while (true) { //we have absolutely no idea how many bytes/frames the file has
		uint32_t frame_header = 0;
		mp3_stream->read((char*)&frame_header, sizeof(frame_header));
		if (frame_header == 0 || mp3_stream->eof())
			break;

		MP3AudioFrameHeader actual_frame_header = convert_binary_to_frame_header(frame_header);

		uint32_t frame_size = get_frame_size(actual_frame_header);
		//printf("ProtectionBit = %02X; BitRate = %u; Frequency = %u; Padding = %02X\n", actual_frame_header.error_protection, get_frame_bitrate(actual_frame_header), get_frame_frequency(actual_frame_header), actual_frame_header.padding_bit);
		//printf("FrameSize = %u\nOffset = %u\n=================\n", frame_size, actual_data_offset);
		mp3_stream->ignore(frame_size - 4);
		actual_data_offset += frame_size;
	}

	return error_code::NONE;
}



MP3AudioFrameHeader MP3Module::convert_binary_to_frame_header(uint32_t frame_header_binary) {
	MP3AudioFrameHeader frame_header;
	frame_header.sync_word = (frame_header_binary & 0x0000D0FF); // not fully correct this one
	frame_header.version = (frame_header_binary & 0x00001800) >> 11;
	frame_header.layer = (frame_header_binary & 0x00000600) >> 9;
	frame_header.error_protection = (frame_header_binary & 0x00000100) >> 8;
	frame_header.bit_rate = (frame_header_binary & 0x00F00000) >> 20;
	frame_header.frequency = (frame_header_binary & 0x000C0000) >> 18;
	frame_header.padding_bit = (frame_header_binary & 0x00020000) >> 17;
	frame_header.private_bit = (frame_header_binary & 0x00010000) >> 16;
	frame_header.mode = (frame_header_binary & 0xC0000000) >> 30;
	frame_header.mode_extension = (frame_header_binary & 0x30000000) >> 28;
	frame_header.copyrighted = (frame_header_binary & 0x08000000) >> 27;
	frame_header.is_original = (frame_header_binary & 0x04000000) >> 26;
	frame_header.emphasis = (frame_header_binary & 0x03000000) >> 24;

	return frame_header;
}


uint32_t MP3Module::convert_frame_header_to_binary(MP3AudioFrameHeader frame_header_structure) {
	uint32_t frame_header = 0;
	frame_header |= 0xFFE00000; //sync_word
	frame_header |= (0x00180000 & ((uint32_t)(frame_header_structure.version)) << 19); //version
	frame_header |= (0x00060000 & ((uint32_t)(frame_header_structure.layer)) << 17); //layer
	frame_header |= (0x00010000 & ((uint32_t)(frame_header_structure.error_protection)) << 16); //protection bit
	frame_header |= (0x0000F000 & ((uint32_t)(frame_header_structure.bit_rate)) << 12); //bit_rate field
	frame_header |= (0x00000C00 & ((uint32_t)(frame_header_structure.frequency)) << 10); //frequency
	frame_header |= (0x00000200 & ((uint32_t)(frame_header_structure.padding_bit)) << 9);
	frame_header |= (0x00000100 & ((uint32_t)(frame_header_structure.private_bit)) << 8);
	frame_header |= (0x000000C0 & (frame_header_structure.mode << 6));
	frame_header |= (0x00000030 & (frame_header_structure.mode_extension << 4));
	frame_header |= (0x00000008 & (frame_header_structure.copyrighted << 3));
	frame_header |= (0x00000004 & (frame_header_structure.is_original << 2));
	frame_header |= (0x00000003 & frame_header_structure.emphasis);

	return (frame_header << 24) |
		((frame_header << 8) & 0x00ff0000) |
		((frame_header >> 8) & 0x0000ff00) |
		((frame_header >> 24) & 0x000000ff);
}

uint32_t MP3Module::get_frame_size(MP3AudioFrameHeader& frame_header) {
	return (144 * get_frame_bitrate(frame_header)) / get_frame_frequency(frame_header) + (frame_header.padding_bit > 0);
}

uint32_t MP3Module::get_frame_bitrate(MP3AudioFrameHeader& frame_header) {
	//most common is MPEG Version 1 Layer 3(thats what we'll focus on, add later more)
	if (frame_header.version == 0x03 && frame_header.layer == 0x01) {
		switch (frame_header.bit_rate) {
		case 0x0E:
			return 320000;
		case 0x0D:
			return 256000;
		case 0x0C:
			return 224000;
		case 0x0B:
			return 192000;
		case 0x0A:
			return 160000;
		case 0x09:
			return 128000;
		case 0x08:
			return 112000;
		case 0x07:
			return 96000;
		case 0x06:
			return 80000;
		case 0x05:
			return 64000;
		case 0x04:
			return 56000;
		case 0x03:
			return 48000;
		case 0x02:
			return 40000;
		case 0x01:
			return 32000;
		default:
			return 0;
		}
	}

	return 0;
}
uint32_t MP3Module::get_frame_frequency(MP3AudioFrameHeader& frame_header) {
	if (frame_header.version == 0x03 && frame_header.layer == 0x01) {
		switch (frame_header.frequency) {
		case 0x00:
			return 44100;
		case 0x01:
			return 48000;
		case 0x02:
			return 32000;
		default:
			return 0;
		}
	}

	return 0;
}
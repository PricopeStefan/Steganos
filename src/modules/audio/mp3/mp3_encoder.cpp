#include <modules/audio/mp3.h>

MP3EncoderModule::MP3EncoderModule(const char* cover_file_path) : MP3Module(cover_file_path) {
	//do nothing if there is no secret provided, just load the BMP into memory
	printf("Created new MP3EncoderModule\n");
}
MP3EncoderModule::MP3EncoderModule(const char* cover_file_path, const char* secret_file_path) : MP3Module(cover_file_path) {
	TRY(utils::load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));
}
MP3EncoderModule::~MP3EncoderModule() {
	//delete the secret data allocated memory
	if (secret_data != nullptr)
		delete[] secret_data;

	if (secret_stream != nullptr)
		delete secret_stream;
}

error_code MP3EncoderModule::image_embed(const MP3ModuleOptions& steg_options) {
	ID3v3APICFrameData apic_custom_frame;
	apic_custom_frame.text_encoding = 0;
	apic_custom_frame.MIME_type = "image/jpeg\0";
	apic_custom_frame.picture_type = 4;
	apic_custom_frame.description = '\0';
	apic_custom_frame.image_data = secret_data;


	ID3v3Frame custom_frame;
	custom_frame.frame_identifier[0] = 'A'; custom_frame.frame_identifier[1] = 'P';
	custom_frame.frame_identifier[2] = 'I'; custom_frame.frame_identifier[3] = 'C';
	uint32_t frame_size = (secret_data_size - 4) + 14; //10 pt image/jpeg mime type, 1 text encoding, 1 picture type, 1 description, -4 pt ca am sizeul in streamul efectiv
	uint8_t frame_size_bytes[4]; utils::convert_uint32_to_bytes(frame_size, frame_size_bytes);
	custom_frame.frame_size[0] = frame_size_bytes[0]; custom_frame.frame_size[1] = frame_size_bytes[1]; 
	custom_frame.frame_size[2] = frame_size_bytes[2]; custom_frame.frame_size[3] = frame_size_bytes[3]; 
	custom_frame.flags[0] = 0; custom_frame.flags[1] = 0; 
	custom_frame.frame_data = (uint8_t*)&apic_custom_frame;

	splice_apic_frame(apic_custom_frame, custom_frame, steg_options);

	return error_code::NONE;
}

error_code MP3EncoderModule::launch_steganos(const MP3ModuleOptions& steg_options) {
	TRY(image_embed(steg_options));

	return error_code::NONE;
}


error_code MP3EncoderModule::splice_apic_frame(ID3v3APICFrameData& apic_custom_frame, ID3v3Frame& custom_frame, const MP3ModuleOptions& options) {
	mp3_stream->seekg(0, std::ios_base::end);
	size_t length = this->cover_size_filesystem;
	printf("Lengthh is %u\n", length);
	mp3_stream->seekg(0, std::ios_base::beg);
	std::vector<char> buff;
	buff.reserve(length);
	std::copy(std::istreambuf_iterator<char>(*this->mp3_stream),
		std::istreambuf_iterator<char>(),
		std::back_inserter(buff));
	
	//writing the first 10 bytes for the ID3 header
	std::ofstream g(options.output_path, std::ios_base::binary);
	uint32_t current_id3_tag_size = utils::convert_synchsafe_uint_to_normal_uint(cover_metadata.header_size);
	current_id3_tag_size += 24 + secret_data_size;
	uint8_t a, b, c, d;
	a = current_id3_tag_size & 0x7f; b = (current_id3_tag_size >> 7) & 0x7f;
	c = (current_id3_tag_size >> 14) & 0x7f; d = (current_id3_tag_size >> 21) & 0x7f;

	cover_metadata.header_size = cover_metadata.header_size | d; cover_metadata.header_size = cover_metadata.header_size | (c << 8);
	cover_metadata.header_size = cover_metadata.header_size | (b << 16); cover_metadata.header_size = cover_metadata.header_size | (a << 24);
	g.write((char*)&cover_metadata, sizeof(MP3MetaStruct));

	//writing the APIC frame first and foremost
	g.write((char*)&custom_frame, sizeof(custom_frame) - sizeof(custom_frame.frame_data));
	g.write((char*)&apic_custom_frame.text_encoding, 1);
	g.write(apic_custom_frame.MIME_type.c_str(), 11);
	g.write((char*)&apic_custom_frame.picture_type, 1);
	g.write(apic_custom_frame.description.c_str(), 1);
	g.write((char*)apic_custom_frame.image_data + 4, secret_data_size - 4);
	//metadata_frames.push_back(custom_frame);

	g.write((char*)&buff[10], buff.size() - 10);

	g.close();

	return error_code::NONE;
}
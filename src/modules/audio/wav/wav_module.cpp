#include <modules/audio/wav.h>
#include <string>

error_code WAVModule::read_cover_metadata() {
	if (wav_stream == nullptr || !wav_stream->is_open())
		return error_code::STREAM_ERROR;

	wav_stream->read((char*)&cover_metadata, sizeof(WAVMetaStruct));

	//smaller than 28 because thats the smallest valid header of a wav file
	if (cover_metadata.chunk_size < 28 || cover_metadata.chunk_id != RIFF_HEADER_ID || cover_metadata.subchunk_1_id != FMT_HEADER_ID) {
		std::cout << "WAV file is broken(invalid riff header/invalid total chunk size/invalid fmt chunk header)" << std::endl;
		return error_code::COVER_FILE_ERROR;
	}

	printf("Total WAV chunk size %u\nSample bit depth %d\n", cover_metadata.chunk_size, cover_metadata.bits_per_sample);
	if (cover_metadata.subchunk_1_size != 16) {
		//probably shouldnt ignore it, have to research more on those extra arguments
		std::cout << "WAVEFORMATEX detected, ignoring " << cover_metadata.subchunk_1_size - 16 << " bytes to get to next chunk" << std::endl;
		for (size_t index = 0; index < cover_metadata.subchunk_1_size - 16; index++) {
			uint8_t index_char = 0;
			wav_stream->read((char*)&index_char, sizeof(uint8_t));
			additional_cover_metadata.push_back(index_char);
		}
	}

	uint32_t next_chunk_id = 0;
	wav_stream->read((char*)&next_chunk_id, sizeof(next_chunk_id));
	while (next_chunk_id != DATA_HEADER_ID && !wav_stream->eof()) {
		//we have another chunk id, possible another format chunk or something else
		wav_stream->read((char*)&next_chunk_id, sizeof(next_chunk_id));
		uint32_t current_chunk_size = 0;
		wav_stream->read((char*)&current_chunk_size, sizeof(current_chunk_size));
		//we aren't actually ignoring the bytes, we save them in a auxiliary vector to just write back
		printf("Ignoring %u bytes to get to the next chunk\n", current_chunk_size);
		
		uint8_t individual_bytes[4];
		memcpy_s(individual_bytes, 4, &next_chunk_id, sizeof(next_chunk_id));
		for (uint8_t index = 0; index < 4; index++)
			additional_cover_metadata.push_back(individual_bytes[index]);
		memcpy_s(individual_bytes, 4, &current_chunk_size, sizeof(current_chunk_size));
		for (uint8_t index = 0; index < 4; index++)
			additional_cover_metadata.push_back(individual_bytes[index]);

		for (size_t index = 0; index < current_chunk_size; index++) {
			uint8_t index_char = 0;
			wav_stream->read((char*)&index_char, sizeof(uint8_t));
			additional_cover_metadata.push_back(index_char);
		}
	}


	return error_code::NONE;
}

error_code WAVModule::read_cover_data() {
	wav_stream->read((char*)&cover_data_length, sizeof(cover_data_length));
	std::cout << "Actual audio data size = " << cover_data_length << std::endl;
	//alocating space for the audio samples
	cover_data = new uint8_t[cover_data_length];

	//TO DO: add checks if stream failed to read
	wav_stream->read(reinterpret_cast<char*>(cover_data), cover_data_length);


	return error_code::NONE;
}


WAVModule::WAVModule(const char* wav_file_path) {
	TRY(utils::load_stream(wav_file_path, wav_stream));

	TRY(read_cover_metadata());
	TRY(read_cover_data());
}

WAVModule::~WAVModule() {
	//deleting the allocated memory for the samples of the cover track
	delete[] cover_data;

	if (wav_stream != nullptr)
		delete wav_stream;

	std::cout << "Deleted audio data and its associated stream" << std::endl;
}

const WAVMetaStruct& WAVModule::get_metadata() const {
	return this->cover_metadata;
}

error_code WAVModule::write_cover(const char* output_path) {
	//to do : add error codes returns if something failed

	std::ofstream embedded_stream(output_path, std::ios_base::binary);
	if (!embedded_stream.is_open())
		return error_code::STREAM_ERROR;

	//TO DO : check if writes succedded
	uint32_t DATA_HEADER = DATA_HEADER_ID;
	embedded_stream.write(reinterpret_cast<char*>(&cover_metadata), sizeof(WAVMetaStruct));
	if (additional_cover_metadata.size() > 0)
		embedded_stream.write(reinterpret_cast<char*>(&additional_cover_metadata[0]), additional_cover_metadata.size());

	embedded_stream.write(reinterpret_cast<char*>(&DATA_HEADER), sizeof(DATA_HEADER));
	embedded_stream.write(reinterpret_cast<char*>(&cover_data_length), sizeof(cover_data_length));

	embedded_stream.write(reinterpret_cast<char*>(cover_data), cover_data_length);

	embedded_stream.close();
	return error_code::NONE;
}
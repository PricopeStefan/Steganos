#include <modules/image/bmp.h>

BMPEncoderModule::BMPEncoderModule(const char* cover_file_path) : BMPModule(cover_file_path) {
	//do nothing if there is no secret provided, just load the BMP into memory
	printf("Created new BMPEncoderModule\n");
}
BMPEncoderModule::BMPEncoderModule(const char* cover_file_path, const char* secret_file_path) : BMPModule(cover_file_path) {
	TRY(utils::load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));
}
BMPEncoderModule::BMPEncoderModule(const char* cover_file_path, const char* secret_file_path, const BMPModuleOptions& steg_options) : BMPModule(cover_file_path) {
	TRY(utils::load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));

	launch_steganos(steg_options);
}

BMPEncoderModule::~BMPEncoderModule() {
	//delete the secret data allocated memory
	if (secret_data != nullptr)
		delete[] secret_data;

	if (secret_stream != nullptr)
		delete secret_stream;
}

error_code BMPEncoderModule::simple_sequential_embed_handler(const BMPModuleOptions& steg_options) {
	//try writing the raw bytes of the secret file
	TRY(simple_sequential_embed(
		sizeof(BGRPixel) * get_padded_width() * cover_image_metadata.height - 32,
		cover_image_data,
		secret_data_size,
		secret_data
	));

	return error_code::NONE;
};

error_code BMPEncoderModule::personal_scramble_embed_handler(const BMPModuleOptions& steg_options) {
	printf("Secret data size = %llu\n", (uint64_t)secret_data_size);
	//try writing the raw bytes of the secret file
	TRY(personal_scramble_embed(
		utils::pixels::types::BGR,
		cover_image_metadata.height,
		get_padded_width(),
		cover_image_data,
		secret_data_size,
		secret_data,
		steg_options.password
	));

	return error_code::NONE;
};

error_code BMPEncoderModule::load_secret(const char* secret_file_path) {
	TRY(utils::load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));

	return error_code::NONE;
}

error_code BMPEncoderModule::launch_steganos(const BMPModuleOptions& steg_options) {
	if (steg_options.compress_secret)
		utils::compress_data(secret_data);

	switch (steg_options.algorithm)
	{
		case BMPModuleSupportedAlgorithms::SEQUENTIAL:
			TRY(simple_sequential_embed_handler(steg_options));
			break;
		case BMPModuleSupportedAlgorithms::PERSONAL_SCRAMBLE:
			TRY(personal_scramble_embed_handler(steg_options));
			break;
		default:
			std::cout << "Algorithm not yet implemented, its on my TO DO\n";
			return error_code::MISC_ERROR;
	}


	TRY(write_bmp());

	return error_code::NONE;
}
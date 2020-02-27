#include <modules/image/bmp.h>

BMPEncoderModule::BMPEncoderModule(const char* cover_file_path, const char* secret_file_path) : BMPModule(cover_file_path) {
	TRY(load_stream(secret_file_path, secret_stream));
	TRY(utils::read_byte_stream(secret_stream, secret_data, secret_data_size));
}
BMPEncoderModule::BMPEncoderModule(const char* cover_file_path, const char* secret_file_path, const BMPModuleOptions& steg_options) : BMPModule(cover_file_path) {
	TRY(load_stream(secret_file_path, secret_stream));
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

error_code BMPEncoderModule::launch_steganos(const BMPModuleOptions& steg_options) {
	if (steg_options.compress_secret)
		utils::compress_data(secret_data);

	switch (steg_options.algorithm)
	{
		case BMPModuleSupportedAlgorithms::SEQUENTIAL:
			TRY(simple_sequential_embed(cover_image_metadata.width, cover_image_metadata.height, cover_image_data, secret_data_size, secret_data));
		case BMPModuleSupportedAlgorithms::PERSONAL_SCRAMBLE:
			TRY(personal_scramble_embed(cover_image_metadata.width, cover_image_metadata.height, cover_image_data, secret_data_size, secret_data));
		default:
			std::cout << "Algorithm not yet implemented, its on my TO DO\n";
			return error_code::MISC_ERROR;
	}

	TRY(write_bmp());

	return error_code::NONE;
}
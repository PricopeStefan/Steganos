#include <modules/image/bmp.h>

error_code BMPDecoderModule::write_secret(const char* output_path) const {
	if (secret_data_size == 0 || secret_data == nullptr) {
		return error_code::DATA_ERROR;
	}

	std::ofstream embedded_stream(output_path, std::ios_base::binary);
	if (!embedded_stream.is_open() || embedded_stream.fail()) {
		return error_code::STREAM_ERROR;
	}

	//TO DO : check if writes succedded
	embedded_stream.write(reinterpret_cast<char*>(secret_data), secret_data_size); 
	embedded_stream.close();

	return error_code::NONE;
}

error_code BMPDecoderModule::sequential_handler(const BMPModuleOptions& steg_options) {
	TRY(simple_sequential_decode(
		sizeof(BGRPixel) * get_padded_width() * cover_image_metadata.height,
		cover_image_data,
		secret_data_size,
		secret_data
	));

	TRY(write_secret(steg_options.output_path.c_str()));

	return error_code::NONE;
}

error_code BMPDecoderModule::personal_scramble_handler(const BMPModuleOptions& steg_options) {
	TRY(personal_scramble_decode(
		utils::pixels::types::BGR,
		cover_image_metadata.height,
		get_padded_width(),
		cover_image_data,
		secret_data_size,
		secret_data,
		steg_options.password
	));

	TRY(write_secret(steg_options.output_path.c_str()));

	return error_code::NONE;
}

BMPDecoderModule::BMPDecoderModule(const char* embedded_path) : BMPModule(embedded_path) {
	//do nothing if no options are given, just load the bmp into memory
}
BMPDecoderModule::BMPDecoderModule(const char* embedded_path, const BMPModuleOptions steg_options) : BMPModule(embedded_path) {
	//image is loaded, try and decode it
	TRY(launch_steganos(steg_options));
}
BMPDecoderModule::~BMPDecoderModule() {
	delete[] secret_data;
}


error_code BMPDecoderModule::launch_steganos(const BMPModuleOptions& steg_options) {
	switch (steg_options.algorithm) {
		case BMPModuleSupportedAlgorithms::SEQUENTIAL:
			return sequential_handler(steg_options);
		case BMPModuleSupportedAlgorithms::PERSONAL_SCRAMBLE:
			return personal_scramble_handler(steg_options);
		default:
			return error_code::MISC_ERROR;
	}
}
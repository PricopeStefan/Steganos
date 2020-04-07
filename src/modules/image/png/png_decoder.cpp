#include <modules/image/png.h>

error_code PNGDecoderModule::write_secret(const char* output_path) const {
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

error_code PNGDecoderModule::sequential_handler(const PNGModuleOptions& steg_options) {
	TRY(simple_sequential_decode(
		image_size,
		image_data,
		secret_data_size,
		secret_data
	));

	TRY(write_secret());

	return error_code::NONE;
}

PNGDecoderModule::PNGDecoderModule(const char* embedded_path) : PNGModule(embedded_path) {
	//do nothing if no options are given, just load the bmp into memory
}
PNGDecoderModule::PNGDecoderModule(const char* embedded_path, const PNGModuleOptions steg_options) : PNGModule(embedded_path) {
	//image is loaded, try and decode it
	TRY(launch_steganos(steg_options));
}
PNGDecoderModule::~PNGDecoderModule() {
	delete[] secret_data;
}


error_code PNGDecoderModule::launch_steganos(const PNGModuleOptions& steg_options) {
	switch (steg_options.algorithm) {
	case PNGModuleSupportedAlgorithms::SEQUENTIAL:
		return sequential_handler(steg_options);
	default:
		return error_code::MISC_ERROR;
	}
}
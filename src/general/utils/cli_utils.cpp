#include <general/utils.h>
#include <modules/image/all.h>
#include <modules/audio/all.h>
#ifdef __linux__ 
#include <experimental/filesystem> 
namespace fs = std::experimental::filesystem;
#elif _WIN32
#include <filesystem>
namespace fs = std::filesystem;
#endif

error_code utils::cli::bmp_encode_handler(cxxopts::ParseResult& args) {
	fs::path cover_path = args["c"].as<std::string>();
	fs::path secret_path = args["s"].as<std::string>();

	BMPEncoderModule encoder_module(cover_path.string().c_str(), secret_path.string().c_str());

	if (args.count("m") > 1) {
		printf("Only one encoding method possible. Please pick one from this list:\n");
		printf("SEQUENTIAL (default)\n");
		printf("PERSONAL_SCRAMBLE\n");
		exit(6);
	}

	BMPModuleOptions options;
	options.password = args["p"].as<std::string>();

	if (args.count("m") == 0 || args["m"].as<std::string>().compare("sequential") == 0)
		options.algorithm = BMPModuleSupportedAlgorithms::SEQUENTIAL;
	else
		options.algorithm = BMPModuleSupportedAlgorithms::PERSONAL_SCRAMBLE;

	options.output_path = args["o"].as<std::string>();
	if (args.count("o") == 0)
		options.output_path = options.output_path + ".bmp";

	//return error_code::NONE;
	return encoder_module.launch_steganos(options);
}

error_code utils::cli::png_encode_handler(cxxopts::ParseResult& args) {
	return error_code::NONE;
}

error_code utils::cli::wav_encode_handler(cxxopts::ParseResult& args) {
	return error_code::NONE;
}

error_code utils::cli::mp3_encode_handler(cxxopts::ParseResult& args) {
	return error_code::NONE;
}




/* DECODE HANDLERS DEFINITIONS */

error_code utils::cli::bmp_decode_handler(cxxopts::ParseResult& args) {
	fs::path cover_path = args["c"].as<std::string>();
	BMPDecoderModule decoder_module(cover_path.string().c_str());

	if (args.count("m") > 1) {
		printf("Only one encoding method possible. Please pick one from this list:\n");
		printf("SEQUENTIAL (default)\n");
		printf("PERSONAL_SCRAMBLE\n");
		exit(6);
	}

	BMPModuleOptions options;
	options.password = args["p"].as<std::string>();

	if (args.count("m") == 0 || args["m"].as<std::string>().compare("sequential") == 0)
		options.algorithm = BMPModuleSupportedAlgorithms::SEQUENTIAL;
	else
		options.algorithm = BMPModuleSupportedAlgorithms::PERSONAL_SCRAMBLE;

	options.output_path = args["o"].as<std::string>();

	//return error_code::NONE;
	return decoder_module.launch_steganos(options);
}


error_code utils::cli::png_decode_handler(cxxopts::ParseResult& args) {
	return error_code::NONE;

}


error_code utils::cli::wav_decode_handler(cxxopts::ParseResult& args) {
	return error_code::NONE;

}


error_code utils::cli::mp3_decode_handler(cxxopts::ParseResult& args) {
	return error_code::NONE;

}
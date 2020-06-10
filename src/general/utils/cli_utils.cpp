#include <general/utils.h>
#include <modules/image/all.h>
#include <modules/audio/all.h>
#include <external/robot36/robot36.h>
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
	
	if (args.count("help")) {
		printf("Available steganography methods when using BMP files as cover:\n");
		printf("SEQUENTIAL (DEFAULT)\n");
		printf("PERSONAL_SCRAMBLE\n");
		printf("Special option if secret file is .ppm : SSTV(and decoded via SEQUENTIAL as a WAV)\n");
		printf("\nExample program usages:\n");
		printf("Encoding(must specify both cover and secret file):\n\t ./Steganos -c cover.bmp -s secret_message.txt -m personal_scramble\n");
		printf("Decoding(must specify only cover file):\n\t ./Steganos -c suspicious_looking.bmp -o what_were_you_hiding.txt\n");
		exit(0);
	}


	if (args.count("m") == 0) {
		std::cout << "Available methods:\n";
		std::cout << "SEQUENTIAL (default)\n";
		std::cout << "PERSONAL_SCRAMBLE\n";
		std::cout << "Special option if secret file is .ppm : SSTV(and decoded via SEQUENTIAL as a WAV)\n";
		std::cout << "No encoding method specified! Picking default option.\n";
	}
	else if (args.count("m") > 1) {
		printf("Only one encoding method possible. Please pick one from this list:\n");
		printf("SEQUENTIAL (default)\n");
		printf("PERSONAL_SCRAMBLE\n");
		printf("Special option if secret file is .ppm : SSTV(and decoded via SEQUENTIAL as a WAV)\n");
		exit(6);
	}

	BMPModuleOptions options;
	options.password = args["p"].as<std::string>();
	
	if (args.count("m") == 0 || args["m"].as<std::string>().compare("sequential") == 0)
		options.algorithm = BMPModuleSupportedAlgorithms::SEQUENTIAL;
	else if (args["m"].as<std::string>().compare("personal_scramble") == 0)
		options.algorithm = BMPModuleSupportedAlgorithms::PERSONAL_SCRAMBLE;
	else if (args["m"].as<std::string>().compare("sstv") == 0 && secret_path.extension().compare(".ppm") == 0) {
		std::string ppm_path_str = secret_path.string();
		char* ppm_path = const_cast<char*>(ppm_path_str.c_str());

		secret_path = fs::temp_directory_path(); secret_path /= "tmp.wav";
		std::string tmp_wav_path_str = secret_path.string();
		char* tmp_wav_path = const_cast<char*>(tmp_wav_path_str.c_str());

		auto sstv_encode_rc = robot36::sstv_encode(ppm_path, tmp_wav_path);
		if (sstv_encode_rc) {
			printf(" Something went wrong with converting to SSTV signal. SSTV RC = %d\n", sstv_encode_rc);
			exit(-1);
		}
	}
	else {
		printf("Invalid method given! Exiting.\n");
		exit(7);
	}

	BMPEncoderModule encoder_module(cover_path.string().c_str(), secret_path.string().c_str());
	options.output_path = args["o"].as<std::string>();
	if (args.count("o") == 0)
		options.output_path = options.output_path + ".bmp";

	return encoder_module.launch_steganos(options);
}

error_code utils::cli::png_encode_handler(cxxopts::ParseResult& args) {
	fs::path cover_path = args["c"].as<std::string>();
	fs::path secret_path = args["s"].as<std::string>();

	if (args.count("help")) {
		printf("Available steganography methods when using BMP files as cover:\n");
		printf("SEQUENTIAL (DEFAULT)\n");
		printf("Special option if secret file is .ppm : SSTV(and decoded via SEQUENTIAL as a WAV)\n");
		printf("\nExample program usages:\n");
		printf("Encoding(must specify both cover and secret file):\n\t ./Steganos -c cover.png -s secret_message.sstv -m sstv\n");
		printf("Decoding(must specify only cover file):\n\t ./Steganos -c suspicious_looking.png -o what_were_you_hiding.wav\n");
		exit(0);
	}


	if (args.count("m") == 0) {
		std::cout << "Available methods:\n";
		std::cout << "SEQUENTIAL (default)\n";
		std::cout << "Special option if secret file is .ppm : SSTV(and decoded via SEQUENTIAL as a WAV)\n";
		std::cout << "No encoding method specified! Picking default option.\n";
	}
	else if (args.count("m") > 1) {
		printf("Only one encoding method possible. Please pick one from this list:\n");
		printf("SEQUENTIAL (default)\n");
		printf("Special option if secret file is .ppm : SSTV(and decoded via SEQUENTIAL as a WAV)\n");
		exit(6);
	}

	PNGModuleOptions options;
	options.password = args["p"].as<std::string>();

	if (args.count("m") == 0 || args["m"].as<std::string>().compare("sequential") == 0)
		options.algorithm = PNGModuleSupportedAlgorithms::SEQUENTIAL;
	else if (args["m"].as<std::string>().compare("sstv") == 0 && secret_path.extension().compare(".ppm") == 0) {
		std::string ppm_path_str = secret_path.string();
		char* ppm_path = const_cast<char*>(ppm_path_str.c_str());

		secret_path = fs::temp_directory_path(); secret_path /= "tmp.wav";
		std::string tmp_wav_path_str = secret_path.string();
		char* tmp_wav_path = const_cast<char*>(tmp_wav_path_str.c_str());

		auto sstv_encode_rc = robot36::sstv_encode(ppm_path, tmp_wav_path);
		if (sstv_encode_rc) {
			printf(" Something went wrong with converting to SSTV signal. SSTV RC = %d\n", sstv_encode_rc);
			exit(-1);
		}
	}
	else {
		printf("Invalid method given! Exiting.\n");
		exit(7);
	}

	PNGEncoderModule encoder_module(cover_path.string().c_str(), secret_path.string().c_str());
	options.output_path = args["o"].as<std::string>();
	if (args.count("o") == 0)
		options.output_path = options.output_path + ".png";

	return encoder_module.launch_steganos(options);
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
	if (args.count("help")) {
		printf("Available steganography methods when using BMP files as cover:\n");
		printf("SEQUENTIAL (DEFAULT)\n");
		printf("PERSONAL_SCRAMBLE\n\n");
		printf("Example program usages:\n");
		printf("Encoding(must specify both cover and secret file):\n\t ./Steganos -c cover.bmp -s secret_message.txt -m personal_scramble\n");
		printf("Decoding(must specify only cover file):\n\t ./Steganos -c suspicious_looking.bmp -o what_were_you_hiding.txt\n");
		exit(0);
	}

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
	else if (args["m"].as<std::string>().compare("personal_scramble") == 0)
		options.algorithm = BMPModuleSupportedAlgorithms::PERSONAL_SCRAMBLE;
	else {
		printf("Invalid method given! Exiting.\n");
		exit(7);
	}

	options.output_path = args["o"].as<std::string>();

	//return error_code::NONE;
	return decoder_module.launch_steganos(options);
}


error_code utils::cli::png_decode_handler(cxxopts::ParseResult& args) {
	if (args.count("help")) {
		printf("Available steganography methods when using BMP files as cover:\n");
		printf("SEQUENTIAL (DEFAULT)\n");
		printf("Example program usages:\n");
		printf("Encoding(must specify both cover and secret file):\n\t ./Steganos -c cover.png -s secret_message.txt -m sequential\n");
		printf("Decoding(must specify only cover file):\n\t ./Steganos -c suspicious_looking.bmp -o what_were_you_hiding.txt\n");
		exit(0);
	}

	fs::path cover_path = args["c"].as<std::string>();
	PNGDecoderModule decoder_module(cover_path.string().c_str());

	if (args.count("m") > 1) {
		printf("Only one encoding method possible. Please pick one from this list:\n");
		printf("SEQUENTIAL (default)\n");
		exit(6);
	}


	PNGModuleOptions options;
	options.password = args["p"].as<std::string>();

	if (args.count("m") == 0 || args["m"].as<std::string>().compare("sequential") == 0)
		options.algorithm = PNGModuleSupportedAlgorithms::SEQUENTIAL;
	else {
		printf("Invalid method given! Exiting.\n");
		exit(7);
	}

	options.output_path = args["o"].as<std::string>();

	return decoder_module.launch_steganos(options);

}


error_code utils::cli::wav_decode_handler(cxxopts::ParseResult& args) {
	return error_code::NONE;

}


error_code utils::cli::mp3_decode_handler(cxxopts::ParseResult& args) {
	return error_code::NONE;

}
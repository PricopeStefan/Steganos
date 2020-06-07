#include <iostream>

#include <general/utils.h>

#ifdef _WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

error_code encode_run_handler(cxxopts::ParseResult args);
error_code decode_run_handler(cxxopts::ParseResult args);

void parse(int argc, char* argv[]) {
	try {
		cxxopts::Options options("Steganos", "Simple steganography project created as a part of my bachelor's thesis");

		options.allow_unrecognised_options()
			.add_options("General")
			("o,output", "Name of the output file", cxxopts::value<std::string>()->default_value("output"), "FILE")
			("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
			("m,method", "The method to be used when encoding/decoding the message", cxxopts::value<std::string>(), "METHOD")
			("p,passkey", "The password used to secure the message or to decipher it", cxxopts::value<std::string>()->default_value("password"), "PASSKEY")
			("h,help", "Prints this help message")
			;

		options.add_options("Encoding/Decoding")
			("c,cover", "The file that serves as a cover for the secret message", cxxopts::value<std::string>(), "FILE")
			("s,secret", "The secret file which will be embedded into the cover file - ENCODING ONLY", cxxopts::value<std::string>(), "FILE")
			;

		auto result = options.parse(argc, argv);
		
		//complete failure cases
		if (result.count("help"))
		{
			std::cout << options.help() << std::endl;
			std::cout << "Example program usages\n";
			std::cout << "Encoding(must specify both cover and secret file): ./Steganos -c cover.bmp -s secret_message.txt\n";
			std::cout << "Decoding(must specify only cover file): ./Steganos -c suspicious_looking.bmp\n";
			exit(0);
		}
		if (result.count("c") != 1) {
			std::cout << "You must specify exactly one cover file." << std::endl;
			std::cout << "Do " << argv[0] << " -h for more information." << std::endl;
			exit(2);
		}

		//verbose redirecting the output in case of argument presence
		std::streambuf* old = std::cout.rdbuf();
		if (result.count("verbose") == 0) {
			std::cout.rdbuf(nullptr);
		}

		//do the code execution
		if (result["s"].count() == 1) {
			TRY(encode_run_handler(result));
		}
		else if (result["s"].count() == 0) {
			TRY(decode_run_handler(result));
		}
		else {
			std::cout << "You must specify only a single secret file." << std::endl;
			std::cout << "Do " << argv[0] << " -h for more information and common usages." << std::endl;
			exit(1);
		}


		//restore the output after execution is done
		if (result.count("verbose") == 0) {
			std::cout.rdbuf(old);
		}

		std::cout << "Done" << std::endl;
	}
	catch (const cxxopts::OptionException& e)
	{
		std::cout << "error parsing options: " << e.what() << std::endl;
		exit(1);
	}

}

int main(int argc, char *argv[]) {
	{
		//MP3Module mp3_module("D:\\Projects\\Steganos\\test_data\\audio\\Vivaldi - Spring-l-dYNttdgl0.mp3");
		//MP3Module mp3_module("D:\\Projects\\Steganos\\test_data\\audio\\Tool - Invincible.mp3");
		//MP3Module mp3_module2("D:\\Projects\\Steganos\\out\\build\\x64-Debug\\output.mp3");
		//MP3EncoderModule encoder("D:\\Projects\\Steganos\\test_data\\audio\\Tool - Invincible.mp3",
		//		"D:\\Projects\\Steganos\\test_data\\secrets\\sstv_me.bmp");
		//
		//encoder.launch_steganos();



		//WAVEncoderModule wav_module("D:\\Projects\\Steganos\\test_data\\audio\\Gavotte_en_rondeau_54.wav", "D:\\Projects\\Steganos\\test_data\\secrets\\orar.xlsx");
		//WAVModuleOptions options;
		//options.number_of_samples_to_skip = 2;
		//wav_module.launch_steganos(options);

		//WAVDecoderModule decoder("D:\\Projects\\Steganos\\out\\build\\x64-Debug\\output.wav");
		//decoder.launch_steganos(options);

		parse(argc, argv);
		/* check if -d option is in argv. if its not try and encode*/
		//encode_run_handler();
		//decode_run_handler();
		//PNGEncoderModule encoder("D:\\Projects\\Steganos\\test_data\\images\\star_field.png", "D:\\Projects\\Steganos\\test_data\\secrets\\orar.xlsx");
		//auto metadata = encoder.get_metadata();
		//printf("Width = %u ; Height = %u; Bit Depth = %d; Color Type = %d; Compression = %d; Filter = %d\n", metadata.width, metadata.height, (int)metadata.bit_depth, (int)metadata.color_type, (int)metadata.compression_method, (int)metadata.filter_method);
		//PNGModuleOptions options;
		//options.algorithm = PNGModuleSupportedAlgorithms::SEQUENTIAL;
		//encoder.launch_steganos(options);

		//PNGDecoderModule decoder("output.png");
		//decoder.launch_steganos(options);
	}

	#ifdef _WIN32
		_CrtDumpMemoryLeaks();
	#endif
	return 0;
}

#include <algorithm>

#ifdef __linux__ 
	#include <experimental/filesystem> 
	namespace fs = std::experimental::filesystem;
#elif _WIN32
	#include <filesystem>
	namespace fs = std::filesystem;
#endif



error_code encode_run_handler(cxxopts::ParseResult args) {
	if (args.count("s") != 1) {
		printf("A single secret file must be also provided.\n");
		printf("Use the -h flag for more information.\n");
		exit(3);
	}
	
	//get cover extension
	std::vector<std::string> valid_extensions = { ".mp3", ".bmp", ".wav", ".png" };
	fs::path cover_path = args["c"].as<std::string>();
	fs::path secret_path = args["s"].as<std::string>();
	if (!fs::exists(cover_path)) {
		printf("Cover file doesn't exist!\n");
		exit(4);
	}
	if (!fs::exists(secret_path)) {
		printf("Secret file doesn't exist!\n");
		exit(4);
	}

	if (std::find(valid_extensions.begin(), valid_extensions.end(), cover_path.extension()) == valid_extensions.end()) {
		printf("Unsupported cover file extension! Please only use BMP, PNG, WAV or MP3 files.\n");
		exit(5);
	}

	if (cover_path.extension().compare(".bmp") == 0)
		return utils::cli::bmp_encode_handler(args);
	else if (cover_path.extension().compare(".png") == 0)
		return utils::cli::png_encode_handler(args);
	else if (cover_path.extension().compare(".wav") == 0)
		return utils::cli::wav_encode_handler(args);
	else if (cover_path.extension().compare(".mp3") == 0)
		return utils::cli::mp3_encode_handler(args);

	return error_code::MISC_ERROR;
}

error_code decode_run_handler(cxxopts::ParseResult args) {
	std::vector<std::string> valid_extensions = { ".mp3", ".bmp", ".wav", ".png" };
	fs::path cover_path = args["c"].as<std::string>();
	if (!fs::exists(cover_path)) {
		printf("Cover file doesn't exist!\n");
		exit(4);
	}
	if (std::find(valid_extensions.begin(), valid_extensions.end(), cover_path.extension()) == valid_extensions.end()) {
		printf("Unsupported cover file extension! Please only use BMP, PNG, WAV or MP3 files.\n");
		exit(5);
	}

	if (cover_path.extension().compare(".bmp") == 0)
		return utils::cli::bmp_decode_handler(args);
	else if (cover_path.extension().compare(".png") == 0)
		return utils::cli::png_decode_handler(args);
	else if (cover_path.extension().compare(".wav") == 0)
		return utils::cli::wav_decode_handler(args);
	else if (cover_path.extension().compare(".mp3") == 0)
		return utils::cli::mp3_decode_handler(args);

	return error_code::MISC_ERROR;
}

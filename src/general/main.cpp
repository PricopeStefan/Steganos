#include <iostream>

#include "modules/image/all.h"
#include <modules/audio/mp3.h>
//#include <external/cxxopts/cxxopts.hpp>

#ifdef _WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

error_code encode_run_handler(/* command line args */) {
	const char* cover_picture = "D:\\Projects\\Steganos\\test_data\\images\\marbles.bmp";
	const char* secret_message = "D:\\Projects\\Steganos\\test_data\\secrets\\orar.xlsx";

	BMPEncoderModule steg_module(cover_picture, secret_message);
	BMPModuleOptions options;
	options.algorithm = BMPModuleSupportedAlgorithms::PERSONAL_SCRAMBLE;

	TRY(steg_module.launch_steganos(options));
	printf("Width = %u; Height = %u\n", steg_module.get_metadata().width, steg_module.get_metadata().height);

	return error_code::NONE;
}
error_code decode_run_handler(/* cmdl args */) {
	BMPDecoderModule desteg_module("D:\\Projects\\Steganos\\out\\build\\x64-Debug\\output.bmp");
	BMPModuleOptions options;
	options.algorithm = BMPModuleSupportedAlgorithms::PERSONAL_SCRAMBLE;

	TRY(desteg_module.launch_steganos(options));

	return error_code::NONE;
}

//void parse(int argc, char* argv[]) {
//	try {
//		cxxopts::Options options("Steganos", "Simple steganography project created as a part of my bachelor's thesis");
//
//		options.allow_unrecognised_options()
//			.add_options("General")
//			("h,help", "Prints this help message")
//			("o,output", "Name of the output file", cxxopts::value<std::string>(), "BIN")
//			("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
//			("a,action", "Steganos action.", cxxopts::value<std::string>()->default_value("encode"), "<encode/decode>")
//			("m,method", "The method to be used when encoding/decoding the message", cxxopts::value<std::string>())
//			;
//
//		options.add_options("Encoding")
//			("cf", "The file to be used as a cover for the secret message", cxxopts::value<std::string>(), "FILE")
//			("sf", "The secret file which will be embedded into the cover file", cxxopts::value<std::string>(), "FILE")
//			;
//
//		options.add_options("Decoding")
//			("if", "The file to be used when decoding", cxxopts::value<std::string>(), "FILE")
//			;
//
//		auto result = options.parse(argc, argv);
//
//		if (result.count("help") || result.arguments().size() == 0)
//		{
//			std::cout << options.help() << std::endl;
//			exit(0);
//		}
//	}
//	catch (const cxxopts::OptionException& e)
//	{
//		std::cout << "error parsing options: " << e.what() << std::endl;
//		exit(1);
//	}
//
//}

int main(int argc, char *argv[]) {
	{
		//MP3Module mp3_module("D:\\Projects\\Steganos\\test_data\\audio\\Vivaldi - Spring-l-dYNttdgl0.mp3");
		//MP3Module mp3_module("D:\\Projects\\Steganos\\test_data\\audio\\Tool - Invincible.mp3");
		//MP3Module mp3_module2("D:\\Projects\\Steganos\\out\\build\\x64-Debug\\output.mp3");
		MP3EncoderModule encoder("D:\\Projects\\Steganos\\test_data\\audio\\Tool - Invincible.mp3",
				"D:\\Projects\\Steganos\\test_data\\secrets\\sstv_me.bmp");
		
		encoder.launch_steganos();



		//WAVEncoderModule wav_module("D:\\Projects\\Steganos\\test_data\\audio\\Gavotte_en_rondeau_54.wav", "D:\\Projects\\Steganos\\test_data\\secrets\\orar.xlsx");
		//WAVModuleOptions options;
		//options.number_of_samples_to_skip = 2;
		//wav_module.launch_steganos(options);

		//WAVDecoderModule decoder("D:\\Projects\\Steganos\\out\\build\\x64-Debug\\output.wav");
		//decoder.launch_steganos(options);

		//parse(argc, argv);
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
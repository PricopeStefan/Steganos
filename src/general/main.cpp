#include <iostream>

#include "modules/image/all.h"

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

int main(int argc, char *argv[]) {
	{
		/* check if -d option is in argv. if its not try and encode*/
		//encode_run_handler();
		//decode_run_handler();

		PNGEncoderModule encoder("D:\\Projects\\Steganos\\test_data\\images\\star_field.png", "D:\\Projects\\Steganos\\test_data\\secrets\\orar.xlsx");
		auto metadata = encoder.get_metadata();
		printf("Width = %u ; Height = %u; Bit Depth = %d; Color Type = %d; Compression = %d; Filter = %d\n", metadata.width, metadata.height, (int)metadata.bit_depth, (int)metadata.color_type, (int)metadata.compression_method, (int)metadata.filter_method);
		PNGModuleOptions options;
		options.algorithm = PNGModuleSupportedAlgorithms::SEQUENTIAL;
		encoder.launch_steganos(options);

		PNGDecoderModule decoder("output.png");
		decoder.launch_steganos(options);
	}
	
	_CrtDumpMemoryLeaks();
	return 0;
}
#include <iostream>

#include "modules/image/bmp.h"

#ifdef _WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

error_code encode_run_handler(/* command line args */) {
	const char* cover_picture = "D:\\Projects\\Steganos\\test_data\\images\\marbles.bmp";
	const char* secret_message = "D:\\Projects\\Steganos\\test_data\\images\\sails.bmp";

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
		encode_run_handler();


		decode_run_handler();
	}
	
	_CrtDumpMemoryLeaks();
	return 0;
}
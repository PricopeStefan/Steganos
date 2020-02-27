#include <iostream>

#include "modules/image/bmp.h"

#ifdef _WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif


int main() {
	{
		const char* cover_picture = "D:\\Facultate\\Licenta\\test_data\\images\\marbles.bmp";
		const char* secret_message = "D:\\Facultate\\Licenta\\test_data\\images\\sails.bmp";
		//const char* secret_message = "D:\\Facultate\\Licenta\\test_data\\secrets\\simple_message.txt";
		BMPEncoderModule steg_module(cover_picture, secret_message);
		steg_module.launch_steganos();
		printf("Width = %u; Height = %u\n", steg_module.get_metadata().width, steg_module.get_metadata().height);


		BMPDecoderModule desteg_module("D:\\Facultate\\Licenta\\out\\build\\x64-Debug\\output.bmp");
		desteg_module.launch_steganos();

	}
	
	_CrtDumpMemoryLeaks();
	return 0;
}
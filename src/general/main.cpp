#include <iostream>

#include "modules/image/bmp.h"

int main() {
	BMPModule steg_module("D:\\Facultate\\Licenta\\test_data\\images\\marbles.bmp", "D:\\Facultate\\Licenta\\test_data\\images\\marbles.bmp");
	printf("Width = %u; Height = %u\n", steg_module.get_cover_width(), steg_module.get_cover_height());
}
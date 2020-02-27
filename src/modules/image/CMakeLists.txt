

add_library(
	BMP_MODULE OBJECT
	"bmp.h"
	"bmp/bmp_module.cpp"
	"bmp/bmp_decoder.cpp"
	"bmp/bmp_encoder.cpp"
)

add_library(
	IMAGE_MODULES
	$<TARGET_OBJECTS:BMP_MODULE>
)

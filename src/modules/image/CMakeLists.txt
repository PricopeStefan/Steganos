

add_library(
	BMP_MODULE OBJECT
	"bmp.h"
	"bmp/bmp_module.cpp"
	"bmp/bmp_decoder.cpp"
	"bmp/bmp_encoder.cpp"
)

add_library(
	PNG_MODULE OBJECT
	"png.h"
	"../../external/lodepng/lodepng.cpp"
	"png/png_module.cpp"
	"png/png_encoder.cpp"
	"png/png_decoder.cpp"
)

add_library(
	IMAGE_ALGORITHMS OBJECT
	${PROJECT_SOURCE_DIR}/src/general/algorithms/simple_sequential.cpp
	${PROJECT_SOURCE_DIR}/src/general/algorithms/personal_scramble.cpp
)

add_library(
	IMAGE_MODULE
	$<TARGET_OBJECTS:IMAGE_ALGORITHMS>
	$<TARGET_OBJECTS:BMP_MODULE>
	$<TARGET_OBJECTS:PNG_MODULE>
)

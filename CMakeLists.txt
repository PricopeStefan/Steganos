
cmake_minimum_required (VERSION 3.8)

project ("Steganos")

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable (
	Steganos
	${PROJECT_SOURCE_DIR}/src/general/main.cpp
	${PROJECT_SOURCE_DIR}/src/general/utils/byte_utils.cpp
	${PROJECT_SOURCE_DIR}/src/general/utils/module_utils.cpp
	${PROJECT_SOURCE_DIR}/src/general/image_algorithms/simple_sequential.cpp
	${PROJECT_SOURCE_DIR}/src/general/image_algorithms/personal_scramble.cpp
)



add_subdirectory(src/modules/image)
target_link_libraries(Steganos IMAGE_MODULES)

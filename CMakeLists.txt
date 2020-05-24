cmake_minimum_required (VERSION 3.6)
set (CMAKE_CXX_STANDARD 17)

project ("Steganos")

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable (
	Steganos
	${PROJECT_SOURCE_DIR}/src/general/main.cpp
	${PROJECT_SOURCE_DIR}/src/general/utils/byte_utils.cpp
)



add_subdirectory(src/modules/image)
add_subdirectory(src/modules/audio)
target_link_libraries(Steganos IMAGE_MODULE AUDIO_MODULE)

if (WIN32)
	target_link_libraries(Steganos wsock32 ws2_32)
endif()

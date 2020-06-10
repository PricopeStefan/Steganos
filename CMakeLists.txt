cmake_minimum_required (VERSION 3.8)


project ("Steganos")

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable (
	Steganos
	${PROJECT_SOURCE_DIR}/src/general/main.cpp
	${PROJECT_SOURCE_DIR}/src/general/utils.h
	${PROJECT_SOURCE_DIR}/src/general/utils/byte_utils.cpp
	${PROJECT_SOURCE_DIR}/src/general/utils/cli_utils.cpp
)

add_subdirectory(src/external/robot36)
add_subdirectory(src/modules/image)
add_subdirectory(src/modules/audio)
target_link_libraries(
	Steganos 
	IMAGE_MODULE 
	AUDIO_MODULE
	robot36
)

if (WIN32)
	target_link_libraries(Steganos wsock32 ws2_32)
endif()

if (UNIX)
    target_link_libraries(Steganos stdc++fs)
endif (UNIX)

target_compile_features(Steganos PRIVATE cxx_std_17)
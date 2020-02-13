
cmake_minimum_required (VERSION 3.8)

project ("Steganos")

set(
	SRC_DIRECTORY
	${PROJECT_SOURCE_DIR}/src
)

set(
	HEADERS
	${SRC_DIRECTORY}/general/module.h
	${SRC_DIRECTORY}/modules/image/bmp.h		
)

add_executable (
	Steganos
	${HEADERS}
	${PROJECT_SOURCE_DIR}/src/general/main.cpp
)

target_include_directories(Steganos PUBLIC
	${PROJECT_SOURCE_DIR}/src
)

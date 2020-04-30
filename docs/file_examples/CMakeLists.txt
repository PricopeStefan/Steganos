cmake_minimum_required (VERSION 3.8)
# Setting the C++ standard to C++17
set (CMAKE_CXX_STANDARD 17)

project ("Steganos")

# Marking the folder where the header files are located
include_directories(${PROJECT_SOURCE_DIR}/src)

# Building the main part of the project and marking it
# as executable
add_executable (
  Steganos
  ${PROJECT_SOURCE_DIR}/src/general/main.cpp
  ${PROJECT_SOURCE_DIR}/src/general/utils/byte_utils.cpp
)

# Adding, building and linking the submodules
add_subdirectory(src/modules/image)
add_subdirectory(src/modules/audio)
target_link_libraries(Steganos IMAGE_MODULE AUDIO_MODULE)

# Platform specific code needed for Windows systems
if (WIN32)
  target_link_libraries(Steganos wsock32 ws2_32)
endif()

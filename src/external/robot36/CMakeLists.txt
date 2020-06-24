set (
	LIBRARY_FILES
	"encode.cpp"
	"img.cpp"
	"buffer.cpp"
	"ddc.cpp"
	"pcm.cpp"
	"ppm.cpp"
	"yuv.cpp"
	"wav.cpp"
	"window.cpp"
	"mmap_file.cpp"
)

if (WIN32)
	set (
		LIBRARY_FILES
		${LIBRARY_FILES}
		"mman.cpp"
	)

	add_library(
		robot36 OBJECT
		${LIBRARY_FILES}
	)
endif()



if (UNIX)
	add_library(
		robot36 STATIC
		${LIBRARY_FILES}
	)

	target_link_libraries(robot36 PRIVATE m stdc++ c)
endif ()

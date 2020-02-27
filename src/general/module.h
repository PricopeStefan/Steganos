#pragma once

#include <fstream>
#include "utils.h"

#define TRY(X) cool_assert((X), __FILE__, __LINE__);

inline void cool_assert(error_code code, const char* file, int line) {
	if (code != error_code::NONE) {
		printf("[Assert error]Message = %d in file %s at line %d\n", code, file, line);
		exit(static_cast<int>(code));
	}
}

class Module {
protected:
	error_code load_stream(const char* file_path, std::ifstream* &stream, std::ios_base::openmode stream_options = std::ios_base::binary) {
		if (stream != nullptr) {
			//stream is already open
			return error_code::STREAM_ERROR;
		}

		stream = new std::ifstream(file_path, stream_options);
		if (stream->is_open()) {
			return error_code::NONE;
		}

		//the opening of the stream failed, deleting it
		delete stream;
		stream = nullptr;

		return error_code::STREAM_ERROR;
	}
};

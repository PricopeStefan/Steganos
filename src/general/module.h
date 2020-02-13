#pragma once

#include <system_error>
#include <fstream>


#define TRY(X) cool_assert((X));

inline void cool_assert(int code) {
	if (code) {
		printf("[Assert error]Message = %d\n", code);
		exit(1);
	}
}

class Module {
protected:
	std::ifstream *cover_stream = nullptr, *secret_stream = nullptr;
public:
	//Used to load the cover multimedia file(the file IN which the content will be hidden)
	//input : the path to the file which will serve as a cover, reference to error code
	//output : error code passed as a reference to the function
	int load_cover(const char* cover_path) {
		std::ifstream *stream = new std::ifstream(cover_path);

		if (stream->is_open()) {
			this->cover_stream = stream;
			return 0;
		}

		return 1;
	}

	//Used to load the secret file(the file which will be hidden)
	//input : the path to the file which will serve as a cover, reference to error code
	//output : error code passed as a reference to the function
	int load_secret(const char* secret_path) {
		std::ifstream* stream = new std::ifstream(secret_path);

		if (stream->is_open()) {
			this->secret_stream = stream;
			return 0;
		}
	
		return 1;
	}

	~Module() {
		if (cover_stream != nullptr)
			delete cover_stream;

		if (secret_stream != nullptr)
			delete secret_stream;
	}
};

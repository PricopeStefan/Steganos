#include <modules/image/bmp.h>

error_code BMPModule::read_cover_metadata() {
	if (bmp_stream == nullptr || !bmp_stream->is_open())
		return error_code::STREAM_ERROR;

	bmp_stream->read((char*)&cover_image_metadata, sizeof(BMPMetaStruct));

	std::cout << "BMP Image data is found beginning at " << cover_image_metadata.data_offset << std::endl;
	if (sizeof(BMPMetaStruct) > cover_image_metadata.data_offset) {
		std::cout << "BMP file is invalid(broken data offset header)" << std::endl;
		return error_code::COVER_FILE_ERROR;
	}

	if (cover_image_metadata.height == 0 || cover_image_metadata.width == 0) {
		std::cout << "BMP file is broken(width/height are 0)" << std::endl;
		return error_code::COVER_FILE_ERROR;
	}

	size_t bytes_until_data = cover_image_metadata.data_offset - sizeof(BMPMetaStruct);
	std::cout << "Ignoring " << bytes_until_data << " bytes to get to the actual image data" << std::endl;
	//really probably shouldn't do this and better research the bmp subvariants and their specific headers
	bmp_stream->ignore(bytes_until_data);

	return error_code::NONE;
}

error_code BMPModule::read_cover_data() {
	//alocating space for the rows of pixels
	cover_image_data = new BGRPixel * [cover_image_metadata.height];

	uint32_t padded_width = cover_image_metadata.width;
	if (cover_image_metadata.width % 4 != 0)
		padded_width += (4 - cover_image_metadata.width % 4);

	//BMP images store data bottom-up : the bottom line is stored as the first in the byte stream
	for (int64_t line_index = cover_image_metadata.height - 1; line_index >= 0; line_index--) {
		//a line of pixels is called a scan line in a BMP file
		BGRPixel* scan_line_pixels = new BGRPixel[padded_width];
		//TO DO: add checks if stream failed to read
		bmp_stream->read(reinterpret_cast<char*>(scan_line_pixels), padded_width * sizeof(BGRPixel));

		cover_image_data[line_index] = scan_line_pixels;
	}

	return error_code::NONE;
}


BMPModule::BMPModule(const char* bmp_file_path) {
	TRY(load_stream(bmp_file_path, bmp_stream));

	TRY(read_cover_metadata());
	TRY(read_cover_data());
}

BMPModule::~BMPModule() {
	//deleting the allocated memory for each line of pixels of the cover image
	for (uint32_t line_index = 0; line_index < cover_image_metadata.height; line_index++) {
		delete[] cover_image_data[line_index];
	}

	delete[] cover_image_data;

	if (bmp_stream != nullptr)
		delete bmp_stream;

	std::cout << "Deleted image data and its associated stream" << std::endl;
}

const BMPMetaStruct& BMPModule::get_metadata() const {
	return this->cover_image_metadata;
}

error_code BMPModule::write_bmp(const char* output_path) {
	//to do : add error codes returns if something failed
	uint32_t padded_width = cover_image_metadata.width;
	if (cover_image_metadata.width % 4 != 0)
		padded_width += (4 - cover_image_metadata.width % 4);

	std::ofstream embedded_stream(output_path, std::ios_base::binary);
	//TO DO : check if writes succedded
	embedded_stream.write(reinterpret_cast<char*>(&cover_image_metadata), sizeof(BMPMetaStruct));

	for (int64_t line_index = cover_image_metadata.height - 1; line_index >= 0; line_index--) {
		BGRPixel* scan_line_pixels = cover_image_data[line_index];
		embedded_stream.write(reinterpret_cast<char*>(scan_line_pixels), padded_width * sizeof(BGRPixel));
	}

	embedded_stream.close();

	return error_code::NONE;
}
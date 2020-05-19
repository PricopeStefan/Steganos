

add_library(
	WAV_MODULE OBJECT
	"wav.h"
	"wav/wav_module.cpp"
	"wav/wav_decoder.cpp"
	"wav/wav_encoder.cpp"
)

add_library(
	MP3_MODULE OBJECT
	"mp3.h"
	"mp3/mp3_module.cpp"
)

add_library(
	AUDIO_MODULE
	$<TARGET_OBJECTS:WAV_MODULE>
	$<TARGET_OBJECTS:MP3_MODULE>
)

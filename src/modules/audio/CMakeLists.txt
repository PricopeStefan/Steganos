

add_library(
	WAV_MODULE OBJECT
	"wav.h"
	"wav/wav_module.cpp"
	"wav/wav_decoder.cpp"
	"wav/wav_encoder.cpp"
)


add_library(
	AUDIO_MODULE
	$<TARGET_OBJECTS:WAV_MODULE>
)

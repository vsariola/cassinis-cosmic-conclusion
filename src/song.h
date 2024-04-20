#pragma once

#define SU_SAMPLE_RATE        44100
#define SU_LENGTH_IN_SAMPLES  (SU_SAMPLE_RATE*144)
#define SU_CHANNEL_COUNT      2
#define SU_BUFFER_LENGTH      (SU_LENGTH_IN_SAMPLES*SU_CHANNEL_COUNT)

using SUsample = float;

extern "C" {
	void __stdcall render_song(float* buffer);
	void __stdcall render_song_main();
}
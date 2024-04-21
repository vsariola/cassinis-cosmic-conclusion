#pragma once

#define STEREO

#define SU_SAMPLE_RATE        44100
#define SU_LENGTH_IN_SECONDS  148
#define SU_LENGTH_IN_SAMPLES  (SU_SAMPLE_RATE*SU_LENGTH_IN_SECONDS)
#ifdef STEREO
#define SU_CHANNEL_COUNT      2
#else
#define SU_CHANNEL_COUNT      1
#endif
#define SU_BUFFER_LENGTH      (SU_LENGTH_IN_SAMPLES*SU_CHANNEL_COUNT)

using SUsample = float;

extern "C" {
	void __stdcall render_song(float* buffer);
	void __stdcall render_song_main();
}
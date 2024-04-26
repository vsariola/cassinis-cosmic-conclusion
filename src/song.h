#ifndef SONG_H
#define SONG_H

#define SU_SAMPLE_RATE        48000
#define SU_LENGTH_IN_SECONDS  148
#define SU_LENGTH_IN_SAMPLES  (SU_SAMPLE_RATE*SU_LENGTH_IN_SECONDS)
#define SU_CHANNEL_COUNT      2
#define SU_BUFFER_LENGTH      (SU_LENGTH_IN_SAMPLES*SU_CHANNEL_COUNT)

using SUsample = short;

#endif

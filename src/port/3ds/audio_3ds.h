#ifndef _3DS_AUDIO_H
#define _3DS_AUDIO_H

#include <3ds.h>
#include "platform_3ds.h"

// Audio configuration
#define AUDIO_SAMPLE_RATE 32000
#define AUDIO_CHANNELS 2
#define AUDIO_BUFFER_SIZE (AUDIO_SAMPLE_RATE / 30)  // ~1/30 second buffer

// N64 AI (Audio Interface) emulation
typedef struct {
    u32 frequency;
    u8 *ram;
    u32 length;
    u32 current_pos;
    s32 remainder;
    u8 enabled;
} N64AudioInterface;

// Initialize audio system
void audio_3ds_init(void);
void audio_3ds_cleanup(void);

// N64 AI emulation
void ai_3ds_init(void);
void ai_3ds_set_frequency(u32 freq);
void ai_3ds_set_length(u32 len);
void ai_3ds_set_ram(u8 *ram);
u32 ai_3ds_get_dma_count(void);
void ai_3ds_reset_dma_count(void);

// Audio playback
void audio_3ds_play_frame(void);
void audio_3ds_submit_samples(const s16 *samples, u32 count);

// Mixer (for multiple audio sources)
typedef struct {
    s16 buffer[AUDIO_BUFFER_SIZE * AUDIO_CHANNELS];
    u32 sample_count;
} AudioBuffer3DS;

void audio_mixer_init(void);
void audio_mixer_add(const s16 *samples, u32 count, u8 volume);
void audio_mixer_output(s16 *output, u32 count);

#endif // _3DS_AUDIO_H

#ifndef _3DS_AUDIO_INTERFACE_H
#define _3DS_AUDIO_INTERFACE_H

#include "platform_3ds.h"
#include "audio_3ds.h"

// N64 Audio Interface (AI) emulation for 3DS
// Connects N64 sound system to 3DS ndsp

// N64 DAC rate (typically 22050 or 32000 Hz)
#define N64_AI_DAC_RATE     22050
#define N64_AI_BIT_RATE     16

// Audio buffer configuration
#define AUDIO_BUFFER_COUNT  2
#define AUDIO_SAMPLES_PER_FRAME  736  // ~22050/30fps

// N64 AI registers (emulated)
typedef struct {
    u32 dramAddr;       // DRAM address of audio data
    u32 length;         // Length of audio data
    u32 control;        // Control register
    u32 dacrate;        // DAC sample rate
    u32 bitrate;        // Bit rate
    u32 nextAddr;       // Next buffer address
    u32 nextLength;     // Next buffer length
} N64AIRegisters;

// Audio DMA buffer
typedef struct {
    s16 *data;
    u32 size;
    u32 samples;
    u8  ready;
} AudioDMABuffer3DS;

// Initialize N64 audio interface emulation
void ai_3ds_interface_init(void);
void ai_3ds_interface_cleanup(void);

// N64 AI register access (for game compatibility)
u32 ai_3ds_get_dram_addr(void);
u32 ai_3ds_get_length(void);
u32 ai_3ds_get_control(void);
u32 ai_3ds_get_dac_rate(void);
u32 ai_3ds_get_bit_rate(void);

void ai_3ds_set_dram_addr(u32 addr);
void ai_3ds_set_length(u32 length);
void ai_3ds_set_control(u32 control);
void ai_3ds_set_dac_rate(u32 rate);
void ai_3ds_set_bit_rate(u32 rate);

// DMA operations
void ai_3ds_dma_read(u32 dramAddr, u32 length);
void ai_3ds_dma_write(u32 dramAddr, u32 length);

// Audio processing
void ai_3ds_process_audio(void);
u32 ai_3ds_get_dma_count(void);
void ai_3ds_reset_dma_count(void);

// Audio output
void ai_3ds_submit_buffer(s16 *data, u32 samples);
u32 ai_3ds_get_buffer_free_space(void);

// Volume control
void ai_3ds_set_master_volume(u8 left, u8 right);
void ai_3ds_get_master_volume(u8 *left, u8 *right);

// Game audio system integration
void audio_3ds_game_init(void);
void audio_3ds_game_update(void);
void audio_3ds_game_play_sound(u32 soundId, u8 volume, u8 pan);
void audio_3ds_game_stop_sound(u32 soundId);
void audio_3ds_game_stop_all_sounds(void);

// Music playback
void audio_3ds_play_music(u32 seqId);
void audio_3ds_stop_music(void);
void audio_3ds_pause_music(void);
void audio_3ds_resume_music(void);
void audio_3ds_set_music_volume(u8 volume);

// Sound effect playback
void audio_3ds_play_sfx(u32 sfxId, u8 volume);
void audio_3ds_play_sfx_pan(u32 sfxId, u8 volume, s8 pan);

// Audio stream callback
void audio_3ds_stream_callback(ndspWaveBuf *waveBuf);

// Global state
N64AIRegisters* ai_3ds_get_registers(void);

#endif // _3DS_AUDIO_INTERFACE_H

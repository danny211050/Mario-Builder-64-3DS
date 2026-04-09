#include <wchar.h>
#include "ai_3ds.h"
#include <string.h>
#include <3ds/types.h>

static N64AIRegisters g_ai_regs;
static AudioDMABuffer3DS g_audio_buffers[AUDIO_BUFFER_COUNT];
static u32 g_current_buffer = 0;
static u32 g_dma_count = 0;
static u8 g_master_vol_left = 255;
static u8 g_master_vol_right = 255;

void ai_3ds_interface_init(void) {
    memset(&g_ai_regs, 0, sizeof(N64AIRegisters));
    
    // Set default DAC rate
    g_ai_regs.dacrate = N64_AI_DAC_RATE;
    g_ai_regs.bitrate = N64_AI_BIT_RATE;
    
    // Allocate audio buffers
    for (int i = 0; i < AUDIO_BUFFER_COUNT; i++) {
        g_audio_buffers[i].data = (s16 *)linearAlloc(AUDIO_SAMPLES_PER_FRAME * 2 * sizeof(s16));
        g_audio_buffers[i].size = AUDIO_SAMPLES_PER_FRAME * 2 * sizeof(s16);
        g_audio_buffers[i].samples = AUDIO_SAMPLES_PER_FRAME;
        g_audio_buffers[i].ready = 0;
    }
    
    // Initialize ndsp if not already done
    audio_3ds_init();
}

void ai_3ds_interface_cleanup(void) {
    // Free audio buffers
    for (int i = 0; i < AUDIO_BUFFER_COUNT; i++) {
        if (g_audio_buffers[i].data) {
            linearFree(g_audio_buffers[i].data);
            g_audio_buffers[i].data = NULL;
        }
    }
}

// Register access
u32 ai_3ds_get_dram_addr(void) { return g_ai_regs.dramAddr; }
u32 ai_3ds_get_length(void) { return g_ai_regs.length; }
u32 ai_3ds_get_control(void) { return g_ai_regs.control; }
u32 ai_3ds_get_dac_rate(void) { return g_ai_regs.dacrate; }
u32 ai_3ds_get_bit_rate(void) { return g_ai_regs.bitrate; }

void ai_3ds_set_dram_addr(u32 addr) {
    g_ai_regs.dramAddr = addr;
    
    // If length is set, start DMA
    if (g_ai_regs.length > 0) {
        ai_3ds_dma_read(addr, g_ai_regs.length);
    }
}

void ai_3ds_set_length(u32 length) {
    g_ai_regs.length = length;
    
    // If address is set, start DMA
    if (g_ai_regs.dramAddr != 0) {
        ai_3ds_dma_read(g_ai_regs.dramAddr, length);
    }
}

void ai_3ds_set_control(u32 control) {
    g_ai_regs.control = control;
}

void ai_3ds_set_dac_rate(u32 rate) {
    g_ai_regs.dacrate = rate;
    
    // Update ndsp sample rate - ndsp expects float
    float ndsp_rate = (float)rate;
    ndspChnSetRate(0, ndsp_rate);
}

void ai_3ds_set_bit_rate(u32 rate) {
    g_ai_regs.bitrate = rate;
}

// DMA operations
void ai_3ds_dma_read(u32 dramAddr, u32 length) {
    // Get current buffer
    AudioDMABuffer3DS *buf = &g_audio_buffers[g_current_buffer];
    
    // Convert N64 DRAM address to local pointer
    // In a real implementation, this would access the N64 memory space
    s16 *n64Audio = (s16 *)dramAddr; // This would need proper memory mapping
    
    // Calculate number of samples
    u32 samples = length / (g_ai_regs.bitrate / 8);
    if (samples > buf->samples) {
        samples = buf->samples;
    }
    
    // Copy audio data with volume scaling
    for (u32 i = 0; i < samples; i++) {
        s32 left = n64Audio[i * 2];
        s32 right = n64Audio[i * 2 + 1];
        
        // Apply master volume
        left = (left * g_master_vol_left) / 255;
        right = (right * g_master_vol_right) / 255;
        
        buf->data[i * 2] = (s16)left;
        buf->data[i * 2 + 1] = (s16)right;
    }
    
    buf->ready = 1;
    
    // Submit to ndsp
    audio_3ds_submit_samples(buf->data, samples);
    
    // Switch buffer
    g_current_buffer = (g_current_buffer + 1) % AUDIO_BUFFER_COUNT;
    
    // Update DMA count
    g_dma_count += samples;
}

void ai_3ds_dma_write(u32 dramAddr, u32 length) {
    // Write audio data to N64 memory (for recording)
    // Not typically used in games
}

// Audio processing
void ai_3ds_process_audio(void) {
    // Called once per frame to process audio
    // Submit any pending audio buffers
    
    for (int i = 0; i < AUDIO_BUFFER_COUNT; i++) {
        AudioDMABuffer3DS *buf = &g_audio_buffers[i];
        if (buf->ready) {
            // Buffer already submitted in DMA read
            buf->ready = 0;
        }
    }
}

u32 ai_3ds_get_dma_count(void) {
    return g_dma_count;
}

void ai_3ds_reset_dma_count(void) {
    g_dma_count = 0;
}

// Audio output
void ai_3ds_submit_buffer(s16 *data, u32 samples) {
    audio_3ds_submit_samples(data, samples);
}

u32 ai_3ds_get_buffer_free_space(void) {
    // Return available buffer space
    return AUDIO_SAMPLES_PER_FRAME;
}

// Volume control
void ai_3ds_set_master_volume(u8 left, u8 right) {
    g_master_vol_left = left;
    g_master_vol_right = right;
    
    // Update ndsp mix - create a simple stereo mix matrix
    float mix[12] = {0};
    mix[0] = (float)left / 255.0f;   // Left input to left output
    mix[1] = (float)right / 255.0f;  // Right input to right output
    ndspChnSetMix(0, mix);
}

void ai_3ds_get_master_volume(u8 *left, u8 *right) {
    *left = g_master_vol_left;
    *right = g_master_vol_right;
}

// Game audio system integration
void audio_3ds_game_init(void) {
    ai_3ds_interface_init();
    
    // Initialize game sound system
    // This would call the game's audio initialization
}

void audio_3ds_game_update(void) {
    // Update audio for current frame
    ai_3ds_process_audio();
    
    // Generate audio for next frame
    // This would call the game's audio synthesis/update function
}

void audio_3ds_game_play_sound(u32 soundId, u8 volume, u8 pan) {
    // Play a game sound effect
    // This interfaces with the game's sound system
}

void audio_3ds_game_stop_sound(u32 soundId) {
    // Stop a specific sound
}

void audio_3ds_game_stop_all_sounds(void) {
    // Stop all sounds
    ndspChnWaveBufClear(0);
}

// Music playback
static u32 g_current_music = 0;
static u8 g_music_volume = 255;
static u8 g_music_paused = 0;

void audio_3ds_play_music(u32 seqId) {
    g_current_music = seqId;
    g_music_paused = 0;
    
    // Start music sequence
    // This would interface with the game's music sequencer
}

void audio_3ds_stop_music(void) {
    g_current_music = 0;
    
    // Stop music
    audio_3ds_game_stop_all_sounds();
}

void audio_3ds_pause_music(void) {
    g_music_paused = 1;
    
    // Pause music sequencer
}

void audio_3ds_resume_music(void) {
    g_music_paused = 0;
    
    // Resume music sequencer
}

void audio_3ds_set_music_volume(u8 volume) {
    float mix[12] = {0};
    mix[0] = volume / 255.0f;  // Left
    mix[1] = volume / 255.0f;  // Right
    ndspChnSetMix(0, mix);
}

// Sound effect playback
void audio_3ds_play_sfx(u32 sfxId, u8 volume) {
    audio_3ds_play_sfx_pan(sfxId, volume, 0);
}

void audio_3ds_play_sfx_pan(u32 soundId, u8 volume, s8 pan) {
    // Calculate left/right volumes based on pan (-64 to +64)
    float left, right;
    float vol = volume / 255.0f;
    
    if (pan < 0) {
        // Pan left
        left = vol;
        right = vol * (64 + pan) / 64.0f;
    } else {
        // Pan right
        left = vol * (64 - pan) / 64.0f;
        right = vol;
    }
    
    float mix[12] = {0};
    mix[0] = left;
    mix[1] = right;
    ndspChnSetMix(0, mix);
    
    // Play sound implementation would go here
}

// Audio stream callback
void audio_3ds_stream_callback(ndspWaveBuf *waveBuf) {
    // Called when ndsp needs more audio data
    // Generate next buffer of audio
}

// Global state
N64AIRegisters* ai_3ds_get_registers(void) {
    return &g_ai_regs;
}

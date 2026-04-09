#include "audio_3ds.h"
#include <string.h>
#include <stdio.h>

// NDSP configuration
#define NDSP_CHANNEL 0
#define NUM_WAVEBUF 2

static ndspWaveBuf g_wave_buf[NUM_WAVEBUF];
static s16 *g_audio_buffer[NUM_WAVEBUF] = {NULL, NULL};
static int g_cur_buf = 0;
static int g_audio_initialized = 0;

// N64 AI emulation
static N64AudioInterface g_ai;
static AudioBuffer3DS g_mixer;

// Audio resampling context (simple linear for now)
typedef struct {
    u32 input_rate;
    u32 output_rate;
    s32 accumulator;
} ResampleContext;

static ResampleContext g_resample;

void audio_3ds_init(void) {
    if (g_audio_initialized) return;
    
    // Initialize ndsp
    ndspInit();
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    
    // Configure channel
    ndspChnReset(NDSP_CHANNEL);
    ndspChnSetFormat(NDSP_CHANNEL, NDSP_FORMAT_STEREO_PCM16);
    ndspChnSetRate(NDSP_CHANNEL, (float)AUDIO_SAMPLE_RATE);
    ndspChnSetInterp(NDSP_CHANNEL, NDSP_INTERP_LINEAR);
    
    // Allocate audio buffers (linear alloc for DMA)
    size_t buf_size = AUDIO_BUFFER_SIZE * AUDIO_CHANNELS * sizeof(s16);
    g_audio_buffer[0] = (s16 *)linearAlloc(buf_size);
    g_audio_buffer[1] = (s16 *)linearAlloc(buf_size);
    
    if (!g_audio_buffer[0] || !g_audio_buffer[1]) {
        printf("Failed to allocate audio buffers\n");
        return;
    }
    
    memset(g_audio_buffer[0], 0, buf_size);
    memset(g_audio_buffer[1], 0, buf_size);
    
    // Initialize wave buffers
    memset(g_wave_buf, 0, sizeof(g_wave_buf));
    
    // Initialize AI emulation
    ai_3ds_init();
    
    // Initialize mixer
    audio_mixer_init();
    
    // Initialize resampling (N64 typically outputs at ~22047 Hz or ~32000 Hz)
    g_resample.input_rate = 22047;
    g_resample.output_rate = AUDIO_SAMPLE_RATE;
    g_resample.accumulator = 0;
    
    g_audio_initialized = 1;
}

void audio_3ds_cleanup(void) {
    if (!g_audio_initialized) return;
    
    // Stop channel
    ndspChnReset(NDSP_CHANNEL);
    
    // Free buffers
    if (g_audio_buffer[0]) {
        linearFree(g_audio_buffer[0]);
        g_audio_buffer[0] = NULL;
    }
    if (g_audio_buffer[1]) {
        linearFree(g_audio_buffer[1]);
        g_audio_buffer[1] = NULL;
    }
    
    ndspExit();
    g_audio_initialized = 0;
}

void ai_3ds_init(void) {
    memset(&g_ai, 0, sizeof(N64AudioInterface));
    g_ai.frequency = 22047;
    g_ai.enabled = 0;
}

void ai_3ds_set_frequency(u32 freq) {
    g_ai.frequency = freq;
    g_resample.input_rate = freq;
    
    // Update ndsp rate
    ndspChnSetRate(NDSP_CHANNEL, (float)AUDIO_SAMPLE_RATE);
}

void ai_3ds_set_length(u32 len) {
    g_ai.length = len;
}

void ai_3ds_set_ram(u8 *ram) {
    g_ai.ram = ram;
    g_ai.current_pos = 0;
    g_ai.enabled = 1;
}

u32 ai_3ds_get_dma_count(void) {
    // Return number of samples played
    return g_ai.current_pos / 4;  // 4 bytes per stereo sample
}

void ai_3ds_reset_dma_count(void) {
    g_ai.current_pos = 0;
}

// Simple linear resampling
static void resample_audio(const s16 *input, u32 input_samples,
                           s16 *output, u32 output_samples,
                           ResampleContext *ctx) {
    for (u32 i = 0; i < output_samples; i++) {
        // Calculate input position
        ctx->accumulator += ctx->input_rate;
        u32 input_pos = ctx->accumulator / ctx->output_rate;
        ctx->accumulator %= ctx->output_rate;
        
        if (input_pos >= input_samples - 1) {
            input_pos = input_samples - 2;
        }
        
        // Linear interpolation
        s32 in0 = input[input_pos * 2];
        s32 in1 = input[(input_pos + 1) * 2];
        s32 frac = ctx->accumulator;
        s32 out_l = in0 + ((in1 - in0) * frac) / ctx->output_rate;
        
        in0 = input[input_pos * 2 + 1];
        in1 = input[(input_pos + 1) * 2 + 1];
        s32 out_r = in0 + ((in1 - in0) * frac) / ctx->output_rate;
        
        output[i * 2] = (s16)out_l;
        output[i * 2 + 1] = (s16)out_r;
    }
}

void audio_3ds_play_frame(void) {
    if (!g_audio_initialized) return;
    
    ndspWaveBuf *buf = &g_wave_buf[g_cur_buf];
    
    // Check if buffer is done
    if (buf->status != NDSP_WBUF_FREE && buf->status != NDSP_WBUF_DONE) {
        return;  // Buffer still playing
    }
    
    // Generate audio data
    s16 *audio_buf = g_audio_buffer[g_cur_buf];
    
    if (g_ai.enabled && g_ai.ram) {
        // Read from N64 audio RAM and resample
        u32 samples_to_read = AUDIO_BUFFER_SIZE / 2;  // Approximate
        s16 temp_buffer[AUDIO_BUFFER_SIZE * 2];
        
        // Copy from N64 RAM (stereo 16-bit samples)
        memcpy(temp_buffer, g_ai.ram + g_ai.current_pos,
               samples_to_read * 4);
        
        g_ai.current_pos += samples_to_read * 4;
        if (g_ai.current_pos >= g_ai.length) {
            g_ai.current_pos = 0;
        }
        
        // Resample to 3DS rate
        resample_audio(temp_buffer, samples_to_read,
                       audio_buf, AUDIO_BUFFER_SIZE,
                       &g_resample);
    } else {
        // Silence
        memset(audio_buf, 0, AUDIO_BUFFER_SIZE * AUDIO_CHANNELS * sizeof(s16));
    }
    
    // Flush cache for DMA
    DSP_FlushDataCache(audio_buf, AUDIO_BUFFER_SIZE * AUDIO_CHANNELS * sizeof(s16));
    
    // Setup wave buffer
    memset(buf, 0, sizeof(ndspWaveBuf));
    buf->data_vaddr = audio_buf;
    buf->nsamples = AUDIO_BUFFER_SIZE;
    buf->looping = false;
    
    // Submit to ndsp
    ndspChnWaveBufAdd(NDSP_CHANNEL, buf);
    
    // Switch buffer
    g_cur_buf = 1 - g_cur_buf;
}

void audio_3ds_submit_samples(const s16 *samples, u32 count) {
    if (!g_audio_initialized) return;
    if (count > AUDIO_BUFFER_SIZE) count = AUDIO_BUFFER_SIZE;
    
    // Copy samples and resample if needed
    memcpy(g_audio_buffer[g_cur_buf], samples,
           count * AUDIO_CHANNELS * sizeof(s16));
    
    // Fill remaining with silence if needed
    if (count < AUDIO_BUFFER_SIZE) {
        memset(g_audio_buffer[g_cur_buf] + count * AUDIO_CHANNELS, 0,
               (AUDIO_BUFFER_SIZE - count) * AUDIO_CHANNELS * sizeof(s16));
    }
}

void audio_mixer_init(void) {
    memset(&g_mixer, 0, sizeof(AudioBuffer3DS));
}

void audio_mixer_add(const s16 *samples, u32 count, u8 volume) {
    if (g_mixer.sample_count + count > AUDIO_BUFFER_SIZE) {
        count = AUDIO_BUFFER_SIZE - g_mixer.sample_count;
    }
    
    // Mix with volume scaling
    for (u32 i = 0; i < count * AUDIO_CHANNELS; i++) {
        s32 mixed = g_mixer.buffer[g_mixer.sample_count * AUDIO_CHANNELS + i];
        mixed += (samples[i] * volume) / 255;
        
        // Clamp
        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;
        
        g_mixer.buffer[g_mixer.sample_count * AUDIO_CHANNELS + i] = (s16)mixed;
    }
    
    g_mixer.sample_count += count;
}

void audio_mixer_output(s16 *output, u32 count) {
    if (count > g_mixer.sample_count) {
        count = g_mixer.sample_count;
    }
    
    memcpy(output, g_mixer.buffer, count * AUDIO_CHANNELS * sizeof(s16));
    
    // Shift remaining samples
    u32 remaining = g_mixer.sample_count - count;
    if (remaining > 0) {
        memmove(g_mixer.buffer,
                g_mixer.buffer + count * AUDIO_CHANNELS,
                remaining * AUDIO_CHANNELS * sizeof(s16));
    }
    
    g_mixer.sample_count = remaining;
}

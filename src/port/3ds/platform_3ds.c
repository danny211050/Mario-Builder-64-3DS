#include "platform_3ds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static u64 g_start_time;
static u32 g_prev_buttons = 0;

void platform_3ds_init(void) {
    // Initialize services
    romfsInit();
    gfxInitDefault();
    
    // Initialize console for debugging (on bottom screen)
    consoleInit(GFX_BOTTOM, NULL);
    
    g_start_time = osGetTime();
}

void platform_3ds_exit(void) {
    platform_3ds_graphics_cleanup();
    platform_3ds_audio_cleanup();
    gfxExit();
    romfsExit();
}

// Graphics implementation
static C3D_RenderTarget *g_top_screen = NULL;
static C3D_RenderTarget *g_bottom_screen = NULL;

void platform_3ds_graphics_init(void) {
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    
    // Create render targets for both screens
    g_top_screen = C3D_RenderTargetCreate(SCREEN_HEIGHT_TOP, SCREEN_WIDTH_TOP, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetClear(g_top_screen, C3D_CLEAR_ALL, 0x000000FF, 0);
    
    g_bottom_screen = C3D_RenderTargetCreate(SCREEN_HEIGHT_BOT, SCREEN_WIDTH_BOT, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetClear(g_bottom_screen, C3D_CLEAR_ALL, 0x000000FF, 0);
}

void platform_3ds_graphics_begin_frame(void) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    
    // Bind top screen (left eye for 3D)
    C3D_FrameDrawOn(g_top_screen);
}

void platform_3ds_graphics_end_frame(void) {
    // Swap buffers
    // C3D_SyncDisplayTransfer(&g_top_screen->frameBuf, GX_BUFFER_DIM(SCREEN_WIDTH_TOP, SCREEN_HEIGHT_TOP),
    //                           (u32 *)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL),
    //                           GX_BUFFER_DIM(SCREEN_WIDTH_TOP, SCREEN_HEIGHT_TOP),
    //                           GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
    //                           GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
    //                           GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));

    C3D_FrameEnd(0);
    gfxSwapBuffersGpu();
    gspWaitForVBlank();
}

void platform_3ds_graphics_cleanup(void) {
    if (g_top_screen) {
        C3D_RenderTargetDelete(g_top_screen);
        g_top_screen = NULL;
    }
    if (g_bottom_screen) {
        C3D_RenderTargetDelete(g_bottom_screen);
        g_bottom_screen = NULL;
    }
    C3D_Fini();
}

// Input implementation
void platform_3ds_input_update(void) {
    hidScanInput();
    g_prev_buttons = hidKeysHeld();
}

u32 platform_3ds_buttons_down(void) {
    return hidKeysDown();
}

u32 platform_3ds_buttons_held(void) {
    return hidKeysHeld();
}

u32 platform_3ds_buttons_up(void) {
    return hidKeysUp();
}

void platform_3ds_touch_read(touchPosition *pos) {
    hidTouchRead(pos);
}

// Audio implementation using ndsp
static ndspWaveBuf g_wave_buf[2];
static s16 *g_audio_buffer[2] = {NULL, NULL};
static int g_cur_buf = 0;

void platform_3ds_audio_init(void) {
    ndspInit();
    
    // Setup audio channel
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);
    ndspChnSetRate(0, 32000); // N64 audio rate
    
    // Allocate audio buffers (2x for double buffering)
    size_t buf_size = 32000 * 2 * sizeof(s16) / 60; // ~1 frame of audio at 60fps
    g_audio_buffer[0] = (s16 *)linearAlloc(buf_size);
    g_audio_buffer[1] = (s16 *)linearAlloc(buf_size);
    
    memset(&g_wave_buf[0], 0, sizeof(g_wave_buf));
}

void platform_3ds_audio_cleanup(void) {
    ndspExit();
    if (g_audio_buffer[0]) linearFree(g_audio_buffer[0]);
    if (g_audio_buffer[1]) linearFree(g_audio_buffer[1]);
}

void platform_3ds_audio_submit_frame(const s16 *data, u32 samples) {
    ndspWaveBuf *buf = &g_wave_buf[g_cur_buf];
    
    // Wait for previous buffer to finish
    if (buf->status != NDSP_WBUF_FREE && buf->status != NDSP_WBUF_DONE) {
        return; // Skip frame if audio is backed up
    }
    
    // Copy data to our buffer
    size_t bytes = samples * 2 * sizeof(s16); // stereo
    memcpy(g_audio_buffer[g_cur_buf], data, bytes);
    
    // Setup wave buffer
    memset(buf, 0, sizeof(ndspWaveBuf));
    buf->data_vaddr = g_audio_buffer[g_cur_buf];
    buf->nsamples = samples;
    buf->looping = false;
    
    // Submit to ndsp
    DSP_FlushDataCache(g_audio_buffer[g_cur_buf], bytes);
    ndspChnWaveBufAdd(0, buf);
    
    g_cur_buf = 1 - g_cur_buf; // Toggle buffer
}

// Timing
u64 platform_3ds_get_time_ms(void) {
    return (osGetTime() - g_start_time) / 1000;
}

void platform_3ds_sleep_ms(u32 ms) {
    svcSleepThread(ms * 1000000LL);
}

void platform_3ds_romfs_init(void) {
    // Already called in platform_3ds_init
}

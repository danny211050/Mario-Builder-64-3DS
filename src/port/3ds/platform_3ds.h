#ifndef _3DS_PLATFORM_H
#define _3DS_PLATFORM_H

// Fix newlib type definitions first
#include "3ds_types_fix.h"

// Standard includes
#include <wchar.h>

// 3DS includes
#include <3ds.h>
#include <citro3d.h>

// Platform detection
#define TARGET_3DS 1

// Screen dimensions for 3DS
#define SCREEN_WIDTH_TOP 400
#define SCREEN_HEIGHT_TOP 240
#define SCREEN_WIDTH_BOT 320
#define SCREEN_HEIGHT_BOT 240

// Game render dimensions (N64 was 320x240 typically)
#define GAME_SCREEN_WIDTH 320
#define GAME_SCREEN_HEIGHT 240

// Platform initialization
void platform_3ds_init(void);
void platform_3ds_exit(void);

// Graphics
void platform_3ds_graphics_init(void);
void platform_3ds_graphics_begin_frame(void);
void platform_3ds_graphics_end_frame(void);
void platform_3ds_graphics_cleanup(void);

// Input
void platform_3ds_input_update(void);
u32 platform_3ds_buttons_down(void);
u32 platform_3ds_buttons_held(void);
u32 platform_3ds_buttons_up(void);
void platform_3ds_touch_read(touchPosition *pos);

// Audio
void platform_3ds_audio_init(void);
void platform_3ds_audio_cleanup(void);
void platform_3ds_audio_submit_frame(const s16 *data, u32 samples);

// Timing
u64 platform_3ds_get_time_ms(void);
void platform_3ds_sleep_ms(u32 ms);

// ROMFS
void platform_3ds_romfs_init(void);

#endif // _3DS_PLATFORM_H

#ifndef _N64_3DS_ADAPTER_H
#define _N64_3DS_ADAPTER_H

#include "n64_compat.h"
#include "platform_3ds.h"
#include "gbi_3ds.h"
#include "game_3ds.h"

// ============================================================================
// N64 to 3DS Main Integration Adapter
// This is the bridge between the N64 game code and the 3DS platform
// ============================================================================

// Main entry point for 3DS - called from main_3ds.c
void n64_3ds_main(void);

// Game loop entry - runs the actual N64 game thread
void n64_3ds_game_thread(void);

// System initialization
void n64_3ds_init_system(void);
void n64_3ds_init_audio(void);
void n64_3ds_init_controllers(void);
void n64_3ds_init_video(void);

// Frame handling
void n64_3ds_frame_begin(void);
void n64_3ds_frame_end(void);
u32 n64_3ds_vsync(void);

// Display list execution
void n64_3ds_run_display_list(Gfx *dl);

// Input handling
void n64_3ds_poll_input(void);
void n64_3ds_update_controllers(void);

// Audio handling
void n64_3ds_audio_frame(void);

// Save handling
void n64_3ds_save_init(void);
void n64_3ds_save_sync(void);

// Memory segments
void n64_3ds_init_segments(void);
void n64_3ds_load_mario_anims(void);
void n64_3ds_load_demo_inputs(void);
void n64_3ds_load_level_entry(void);

// ROMFS asset loading
void n64_3ds_load_romfs_assets(void);

// Game state
extern u32 g_3ds_frame_count;
extern u32 g_3ds_last_frame_time;
extern u8  g_3ds_running;

// Thread synchronization
void n64_3ds_yield_thread(void);
void n64_3ds_wait_vblank(void);

#endif // _N64_3DS_ADAPTER_H

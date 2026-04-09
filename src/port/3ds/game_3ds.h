#ifndef _3DS_GAME_H
#define _3DS_GAME_H

#include "PR/ultratypes.h"
#include "platform_3ds.h"
#include "gbi_3ds.h"
#include "input_3ds.h"

// 3DS Game loop configuration
#define TARGET_FPS 30
#define FRAME_TIME_MS (1000 / TARGET_FPS)

// Game states
typedef enum {
    GAME_STATE_INIT,
    GAME_STATE_TITLE,
    GAME_STATE_FILE_SELECT,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSE,
    GAME_STATE_BUILDER,
    GAME_STATE_EXIT
} GameState3DS;

// 3DS-specific game context
typedef struct {
    GameState3DS state;
    u32 frameCount;
    u64 lastFrameTime;
    u64 accumulatedTime;
    
    // Screen management
    u8 activeScreen; // 0 = top, 1 = bottom
    u8 is3DEnabled;
    
    // Builder mode touch state
    u8 builderTouchActive;
    u16 builderTouchX;
    u16 builderTouchY;
    u8 builderToolSelected;
    
    // Performance tracking
    f32 fps;
    u32 frameTimeUs;
    
    // Save data state
    u8 saveLoaded;
    u8 saveDirty;
} GameContext3DS;

// Main game initialization for 3DS
void game_3ds_init(void);
void game_3ds_cleanup(void);

// Main game loop
void game_3ds_loop(void);
u32 game_3ds_should_exit(void);

// Frame management
void game_3ds_frame_begin(void);
void game_3ds_frame_end(void);
void game_3ds_frame_wait(void);

// Game state management
void game_3ds_set_state(GameState3DS state);
GameState3DS game_3ds_get_state(void);

// Screen management
void game_3ds_set_active_screen(u8 screen);
void game_3ds_render_top_screen(void);
void game_3ds_render_bottom_screen(void);

// Builder mode (touch screen)
void game_3ds_builder_init(void);
void game_3ds_builder_update(void);
void game_3ds_builder_render(void);
void game_3ds_builder_handle_touch(u16 x, u16 y, u8 pressed);

// Input processing
void game_3ds_process_input(void);
void game_3ds_process_n64_input(void); // Translate to N64 format
void game_3ds_process_touch_input(void);

// Save/Load
void game_3ds_save_game(void);
void game_3ds_load_game(void);

// Audio integration
void game_3ds_audio_init(void);
void game_3ds_audio_update(void);

// Performance
f32 game_3ds_get_fps(void);
u32 game_3ds_get_frame_time(void);

// Global context accessor
GameContext3DS* game_3ds_get_context(void);

#endif // _3DS_GAME_H

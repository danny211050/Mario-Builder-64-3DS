#include "game_3ds.h"
#include "audio_3ds.h"
#include "romfs_3ds.h"
#include "save_3ds.h"
#include <stdio.h>

static GameContext3DS g_game_context;

void game_3ds_init(void) {
    memset(&g_game_context, 0, sizeof(GameContext3DS));
    
    printf("Initializing Mario Builder 64 for 3DS...\n");
    
    // Initialize all subsystems
    platform_3ds_init();
    platform_3ds_graphics_init();
    platform_3ds_audio_init();
    audio_3ds_init();
    input_3ds_init();
    gbi_3ds_init();
    
    // Initialize ROMFS
    romfs_3ds_init();
    
    // Initialize save system
    save_3ds_init();
    
    // Set initial state
    g_game_context.state = GAME_STATE_INIT;
    g_game_context.activeScreen = 0; // Start on top screen
    g_game_context.is3DEnabled = 0;
    
    // Timing
    g_game_context.lastFrameTime = platform_3ds_get_time_ms();
    
    printf("Initialization complete!\n");
}

void game_3ds_cleanup(void) {
    printf("Cleaning up...\n");
    
    // Save if needed
    if (g_game_context.saveDirty) {
        game_3ds_save_game();
    }
    
    // Cleanup subsystems
    gbi_3ds_cleanup();
    audio_3ds_cleanup();
    save_3ds_cleanup();
    romfs_3ds_exit();
    
    platform_3ds_exit();
}

void game_3ds_loop(void) {
    while (!game_3ds_should_exit()) {
        // Frame timing
        game_3ds_frame_begin();
        
        // Input
        game_3ds_process_input();
        
        // Update
        switch (g_game_context.state) {
            case GAME_STATE_INIT:
                // Load initial data
                game_3ds_load_game();
                g_game_context.state = GAME_STATE_TITLE;
                break;
                
            case GAME_STATE_TITLE:
                // Title screen update
                break;
                
            case GAME_STATE_FILE_SELECT:
                // File select update
                break;
                
            case GAME_STATE_PLAYING:
                // Main game update
                break;
                
            case GAME_STATE_PAUSE:
                // Pause menu update
                break;
                
            case GAME_STATE_BUILDER:
                game_3ds_builder_update();
                break;
                
            case GAME_STATE_EXIT:
                return;
        }
        
        // Audio
        game_3ds_audio_update();
        
        // Render
        game_3ds_render_top_screen();
        game_3ds_render_bottom_screen();
        
        // Frame end
        game_3ds_frame_end();
        
        g_game_context.frameCount++;
    }
}

u32 game_3ds_should_exit(void) {
    // Check for exit conditions
    u32 buttons = platform_3ds_buttons_held();
    
    // Exit if START + SELECT pressed
    if ((buttons & KEY_START) && (buttons & KEY_SELECT)) {
        return 1;
    }
    
    return 0;
}

void game_3ds_frame_begin(void) {
    platform_3ds_graphics_begin_frame();
    
    u64 currentTime = platform_3ds_get_time_ms();
    g_game_context.frameTimeUs = (u32)(currentTime - g_game_context.lastFrameTime);
    g_game_context.lastFrameTime = currentTime;
}

void game_3ds_frame_end(void) {
    platform_3ds_graphics_end_frame();
}

void game_3ds_frame_wait(void) {
    // Frame rate limiting
    u64 frameTime = platform_3ds_get_time_ms() - g_game_context.lastFrameTime;
    if (frameTime < FRAME_TIME_MS) {
        platform_3ds_sleep_ms((u32)(FRAME_TIME_MS - frameTime));
    }
}

void game_3ds_set_state(GameState3DS state) {
    g_game_context.state = state;
}

GameState3DS game_3ds_get_state(void) {
    return g_game_context.state;
}

void game_3ds_process_input(void) {
    // Update input state
    platform_3ds_input_update();
    input_3ds_update();
    
    // Process N64 controller input
    game_3ds_process_n64_input();
    
    // Process touch input for builder mode
    game_3ds_process_touch_input();
}

void game_3ds_process_n64_input(void) {
    // Get N64 button mapping
    u32 n64_buttons = input_3ds_get_n64_buttons();
    s8 stick_x, stick_y;
    input_3ds_get_n64_stick(&stick_x, &stick_y);
    
    // TODO: Pass to game's input system
    // This would interface with the existing N64 input code
}

void game_3ds_process_touch_input(void) {
    const InputState* state = input_3ds_get_state();
    
    if (g_game_context.state == GAME_STATE_BUILDER) {
        if (state->touch_pressed || state->touch_held) {
            game_3ds_builder_handle_touch(
                state->touch.px,
                state->touch.py,
                state->touch_pressed
            );
        }
    }
}

void game_3ds_render_top_screen(void) {
    // Set viewport for top screen
    C3D_SetViewport(0, 0, SCREEN_WIDTH_TOP, SCREEN_HEIGHT_TOP);
    
    // Clear
    u32 clearColor = 0xFF6495ED; // Sky blue
    // Skip C3D_RenderTargetClear - use simple clear for now
    
    // Render based on state
    switch (g_game_context.state) {
        case GAME_STATE_TITLE:
            // Render title screen
            break;
            
        case GAME_STATE_PLAYING:
            // Render game world
            break;
            
        case GAME_STATE_BUILDER:
            // Render builder 3D view
            break;
            
        default:
            break;
    }
}

void game_3ds_render_bottom_screen(void) {
    // Switch to bottom screen render target
    // Note: In practice, this would use a different render target
    
    // Clear
    u32 clearColor = 0xFF202020; // Dark grey
    // Skip C3D_RenderTargetClear - use simple clear for now
    
    // Render based on state
    switch (g_game_context.state) {
        case GAME_STATE_BUILDER:
            game_3ds_builder_render();
            break;
            
        default:
            // Render menu or minimap
            break;
    }
}

void game_3ds_builder_init(void) {
    g_game_context.builderTouchActive = 0;
    g_game_context.builderToolSelected = 0;
}

void game_3ds_builder_update(void) {
    // Update builder state
}

void game_3ds_builder_render(void) {
    // Render builder UI on bottom screen
    // - Tool palette on left
    // - Block selector on right
    // - Minimap in center
    
    // This would render 2D UI elements
}

void game_3ds_builder_handle_touch(u16 x, u16 y, u8 pressed) {
    g_game_context.builderTouchX = x;
    g_game_context.builderTouchY = y;
    
    if (pressed) {
        g_game_context.builderTouchActive = 1;
        
        // Handle touch based on screen region
        if (x < 80) {
            // Left side - tools
            g_game_context.builderToolSelected = y / 40;
        } else if (x > 240) {
            // Right side - blocks
        } else {
            // Center - 3D view interaction
        }
    } else {
        g_game_context.builderTouchActive = 0;
    }
}

void game_3ds_save_game(void) {
    // Save game data
    extern u8 gSaveBuffer[]; // From game's save system
    save_3ds_write(gSaveBuffer, 0x200);
    g_game_context.saveDirty = 0;
}

void game_3ds_load_game(void) {
    // Load game data
    extern u8 gSaveBuffer[];
    if (save_3ds_read(gSaveBuffer, 0x200) == 0) {
        g_game_context.saveLoaded = 1;
    }
}

void game_3ds_audio_init(void) {
    // Audio initialized in platform init
}

void game_3ds_audio_update(void) {
    // Update audio stream
    // This would process game audio and submit to ndsp
}

f32 game_3ds_get_fps(void) {
    if (g_game_context.frameTimeUs > 0) {
        return 1000000.0f / g_game_context.frameTimeUs;
    }
    return 0.0f;
}

u32 game_3ds_get_frame_time(void) {
    return g_game_context.frameTimeUs;
}

GameContext3DS* game_3ds_get_context(void) {
    return &g_game_context;
}

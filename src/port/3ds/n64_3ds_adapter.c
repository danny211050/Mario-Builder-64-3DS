#include "types.h"
#include "game_init.h"
#include "n64_3ds_adapter.h"
#include "audio_3ds.h"
#include "ai_3ds.h"
#include "input_3ds.h"
#include "save_3ds.h"
#include "asset_loader.h"
#include "touch_ui.h"
#include <stdio.h>
#include <string.h>

// Game state
u32 g_3ds_frame_count = 0;
u32 g_3ds_last_frame_time = 0;
u8  g_3ds_running = 1;

// External game functions
extern void thread5_game_loop(void *arg);
extern void setup_game_memory(void);
extern void init_controllers(void);
extern void render_init(void);
extern void select_gfx_pool(void);
extern void read_controller_inputs(s32 threadID);
extern void audio_game_loop_tick(void);
extern struct LevelCommand *level_script_execute(struct LevelCommand *addr);
extern void display_and_vsync(void);
extern void end_master_display_list(void);
extern void init_rcp(s32 resetZB);
extern void clear_framebuffer(s32 color);

// Game globals
extern struct GfxPool *gGfxPool;
extern Gfx *gDisplayListHead;
extern u32 gGlobalTimer;
extern u8 gBorderHeight;
extern struct LevelCommand *addr;

// Main entry point
void n64_3ds_main(void) {
    printf("Mario Builder 64 - 3DS Port\n");
    printf("============================\n\n");
    
    // Initialize all 3DS subsystems
    platform_3ds_init();
    
    // Initialize game systems
    n64_3ds_init_system();
    
    printf("Starting game loop...\n");
    
    // Run the game
    n64_3ds_game_thread();
}

// Game thread - this replaces the N64 thread5_game_loop
void n64_3ds_game_thread(void) {
    // Setup game memory
    setup_game_memory();
    
    // Initialize controllers
    init_controllers();
    
    // Initialize audio
    n64_3ds_init_audio();
    audio_3ds_game_init();
    
    // Initialize video/rendering
    n64_3ds_init_video();
    
    // Load ROMFS assets
    n64_3ds_load_romfs_assets();
    
    // Initial render
    render_init();
    
    // Main game loop
    extern void *level_script_entry;
    void *addr = segmented_to_virtual(level_script_entry);
    
    while (g_3ds_running) {
        // Frame begin
        n64_3ds_frame_begin();
        
        // Poll input
        n64_3ds_poll_input();
        
        // Audio
        audio_game_loop_tick();
        n64_3ds_audio_frame();
        
        // Select graphics pool
        select_gfx_pool();
        
        // Update controllers (called directly instead of via message queue)
        read_controller_inputs(5); // THREAD_5_GAME_LOOP
        
        // Execute level script (main game logic)
        addr = level_script_execute(addr);
        
        // End display list
        end_master_display_list();
        
        // Execute display list
        // n64_3ds_exec_display_list(&gGfxPool->spTask); // Not used on 3DS
        
        // Frame end / VSync
        n64_3ds_frame_end();
        
        g_3ds_frame_count++;
        gGlobalTimer++;
    }
}

// System initialization
void n64_3ds_init_system(void) {
    printf("Initializing N64/3DS adapter system...\n");
    
    // Initialize N64 compatibility layer
    // (done in n64_compat.c)
    
    // Initialize save system
    n64_3ds_save_init();
    
    // Initialize touch UI for builder mode
    touch_ui_init();
}

void n64_3ds_init_audio(void) {
    printf("Initializing audio...\n");
    audio_3ds_init();
    ai_3ds_interface_init();
}

void n64_3ds_init_controllers(void) {
    printf("Initializing controllers...\n");
    input_3ds_init();
    
    // Setup N64 controller emulation
    gControllerBits = 0x01; // Controller 1 present
}

void n64_3ds_init_video(void) {
    printf("Initializing video...\n");
    platform_3ds_graphics_init();
    gbi_3ds_init();
    
    // Set up framebuffers
    gBorderHeight = 0;
}

// Frame handling
void n64_3ds_frame_begin(void) {
    // Set up render target for top screen
    platform_3ds_graphics_begin_frame();
    
    // Initialize RCP/RDP
    init_rcp(1); // CLEAR_ZBUFFER
    
    // Clear framebuffer
    clear_framebuffer(0x00000000); // Black clear
}

void n64_3ds_frame_end(void) {
    // Swap buffers
    platform_3ds_graphics_end_frame();
    
    // VSync - wait for 30fps (approx 2 vblanks at 60Hz)
    n64_3ds_vsync();
}

u32 n64_3ds_vsync(void) {
    // Wait for vblank
    platform_3ds_vblank_wait();
    return 0;
}

// Display list execution
// void n64_3ds_exec_display_list(SPTask *task) {
//     (void)task;
// }

void n64_3ds_run_display_list(Gfx *dl) {
    if (dl) {
        gbi_3ds_run_dl((N64Gfx *)dl);
    }
}

// Input handling
void n64_3ds_poll_input(void) {
    // Update 3DS input state
    platform_3ds_input_update();
    input_3ds_update();
}

void n64_3ds_update_controllers(void) {
    // Update N64 controller data from 3DS input
    u16 n64_buttons = input_3ds_get_n64_buttons();
    s8 stick_x, stick_y;
    platform_3ds_get_stick(&stick_x, &stick_y);
    
    (void)n64_buttons;
    (void)stick_x;
    (void)stick_y;
}

// Audio handling
void n64_3ds_audio_frame(void) {
    // Process audio for current frame
    ai_3ds_process_audio();
    
    // Submit any pending audio buffers
    audio_3ds_game_update();
}

// void n64_3ds_handle_audio_task(SPTask *task) {
//     (void)task;
// }

// Save handling
void n64_3ds_save_init(void) {
    save_3ds_init();
}

void n64_3ds_save_sync(void) {
    // Sync save data to SD card
}

// Memory segments
void n64_3ds_init_segments(void) {
    // Initialize segment base addresses
    set_segment_base_addr(SEGMENT_MAIN, (void *)0x80000000);
}

void n64_3ds_load_mario_anims(void) {
    // Load Mario animations from ROMFS
    void *animData = asset_loader_file_read("anims/mario.anims", NULL);
    if (animData) {
        set_segment_base_addr(SEGMENT_MARIO_ANIMS, animData);
    }
}

void n64_3ds_load_demo_inputs(void) {
    // Load demo inputs from ROMFS
}

void n64_3ds_load_level_entry(void) {
    // Load level entry segment from ROMFS
}

// ROMFS asset loading
void n64_3ds_load_romfs_assets(void) {
    printf("Loading ROMFS assets...\n");
    
    asset_loader_3ds_init();
    
    // Load critical assets
    n64_3ds_load_mario_anims();
    n64_3ds_load_demo_inputs();
    n64_3ds_load_level_entry();
    
    printf("ROMFS assets loaded.\n");
}

// Thread synchronization
void n64_3ds_yield_thread(void) {
    // Yield to 3DS scheduler
    svcSleepThread(1000);
}

void n64_3ds_wait_vblank(void) {
    platform_3ds_vblank_wait();
}

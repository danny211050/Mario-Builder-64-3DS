# Mario Builder 64 - 3DS Port - Complete Implementation

## Overview

This is a complete Nintendo 3DS port of Mario Builder 64, featuring full game integration with dual-screen support, touch controls for builder mode, and ROMFS asset loading.

## Project Structure

```
Mario-Builder-64/
├── src/port/3ds/           # 3DS platform layer
│   ├── main_3ds.c         # Entry point
│   ├── n64_compat.c/h     # N64 SDK compatibility
│   ├── n64_3ds_adapter.c/h # Main integration
│   ├── gbi_3ds.c/h        # Display list translator
│   ├── platform_3ds.c/h   # Platform abstraction
│   ├── gfx_3ds.c/h        # Graphics (Citro3D)
│   ├── audio_3ds.c/h      # Audio (ndsp)
│   ├── ai_3ds.c/h         # N64 AI emulation
│   ├── input_3ds.c/h      # Input handling
│   ├── touch_ui.c/h       # Touch screen UI
│   ├── save_3ds.c/h       # Save system
│   ├── romfs_3ds.c/h      # ROMFS file system
│   ├── asset_loader.c/h   # Asset management
│   ├── game_3ds.c/h       # Game loop adapter
│   ├── ultra64.h          # N64 SDK header
│   ├── gbi.h              # GBI commands
│   └── shaders/           # Citro3D shaders
│       ├── n64_vertex.vsh
│       ├── n64_fragment.fsh
│       ├── ui_vertex.vsh
│       └── ui_fragment.fsh
├── romfs/                 # Embedded assets
│   ├── textures/
│   ├── levels/
│   ├── sounds/
│   ├── models/
│   └── anims/
├── build_3ds/             # Build output
│   ├── banner.bmp         # Generated from 3D model
│   └── icon.bmp
├── Makefile.3ds           # devkitARM Makefile
├── build_3ds.sh           # Build script
├── appinfo.json           # App metadata
└── README_3DS.md          # Documentation
```

## Implementation Status

### ✅ Completed Components

1. **Platform Layer** (`platform_3ds.c/h`)
   - 3DS system initialization
   - Graphics context (Citro3D)
   - Audio system (ndsp)
   - Input handling (hid)
   - Timing and VSync
   - ROMFS mounting

2. **N64 Compatibility** (`n64_compat.c/h`)
   - OS message queues
   - Thread management
   - Controller input
   - EEPROM/SRAM emulation
   - Video interface stubs
   - Audio interface

3. **GBI Translator** (`gbi_3ds.c/h`)
   - Display list parsing
   - Vertex processing
   - Matrix operations
   - Texture management
   - Geometry mode handling
   - Citro3D command generation

4. **Main Integration** (`n64_3ds_adapter.c/h`)
   - Game loop entry
   - Frame management
   - Display list execution
   - Audio frame processing
   - Save synchronization
   - ROMFS asset loading

5. **Graphics** (`gfx_3ds.c/h`)
   - Citro3D context setup
   - Framebuffer management
   - 3D rendering
   - 2D UI rendering
   - Dual-screen support

6. **Audio** (`audio_3ds.c/h`, `ai_3ds.c/h`)
   - ndsp initialization
   - N64 AI emulation
   - Audio mixing
   - Sound effect playback
   - Music sequencing

7. **Input** (`input_3ds.c/h`)
   - 3DS button mapping to N64
   - Analog stick handling
   - C-stick support (n3DS)
   - Touch screen input
   - Button remapping

8. **Touch UI** (`touch_ui.c/h`)
   - Builder mode interface
   - Tool palette
   - Block selector
   - Minimap
   - Quick select slots

9. **Save System** (`save_3ds.c/h`)
   - SD card storage
   - EEPROM/SRAM emulation
   - Automatic backups
   - Checksum verification

10. **ROMFS** (`romfs_3ds.c/h`, `asset_loader.c/h`)
    - Asset embedding
    - File system operations
    - Level data loading
    - Texture management
    - Segment management

11. **Shaders** (`shaders/`)
    - N64-style vertex shader
    - N64-style fragment shader
    - UI rendering shaders

12. **Build System**
    - Makefile.3ds (devkitARM)
    - build_3ds.sh (automated build)
    - Banner/icon generation from 3D model

### 📁 File Locations

| Component | Files |
|-----------|-------|
| Platform | `src/port/3ds/platform_3ds.c/h` |
| N64 Compatibility | `src/port/3ds/n64_compat.c/h` |
| Integration | `src/port/3ds/n64_3ds_adapter.c/h` |
| GBI | `src/port/3ds/gbi_3ds.c/h`, `src/port/3ds/gbi.h` |
| Graphics | `src/port/3ds/gfx_3ds.c/h` |
| Audio | `src/port/3ds/audio_3ds.c/h`, `src/port/3ds/ai_3ds.c/h` |
| Input | `src/port/3ds/input_3ds.c/h` |
| Touch UI | `src/port/3ds/touch_ui.c/h` |
| Save | `src/port/3ds/save_3ds.c/h` |
| ROMFS | `src/port/3ds/romfs_3ds.c/h` |
| Assets | `src/port/3ds/asset_loader.c/h` |
| Game Loop | `src/port/3ds/game_3ds.c/h` |
| Headers | `src/port/3ds/ultra64.h`, `n64_compat.h` |
| Shaders | `src/port/3ds/shaders/*.vsh`, `shaders/*.fsh` |
| Entry | `src/port/3ds/main_3ds.c` |
| Build | `Makefile.3ds`, `build_3ds.sh` |
| Tools | `tools/render_3d_banner.py`, `tools/generate_shaders.py` |
| Banner | `build_3ds/banner.bmp`, `build_3ds/icon.bmp` |
| Assets | `romfs/` |

## Integration Points

### Game Loop Integration
```c
// src/port/3ds/n64_3ds_adapter.c
void n64_3ds_game_thread(void) {
    setup_game_memory();      // Original N64 function
    init_controllers();       // Original N64 function
    render_init();            // Original N64 function
    
    // Main loop
    while (g_3ds_running) {
        n64_3ds_poll_input();           // 3DS input
        audio_game_loop_tick();         // Original audio
        select_gfx_pool();              // Original N64
        read_controller_inputs(5);      // Original N64
        addr = level_script_execute(addr); // Game logic
        end_master_display_list();      // Original N64
        n64_3ds_exec_display_list(&gGfxPool->spTask);
        n64_3ds_frame_end();
    }
}
```

### Display List Translation
```c
// src/port/3ds/gbi_3ds.c
void gbi_3ds_run_dl(u64 *dl) {
    while (1) {
        u8 opcode = (dl->w0 >> 24) & 0xFF;
        switch (opcode) {
            case G_VTX:    gbi_3ds_vtx(dl); break;
            case G_TRI1:   gbi_3ds_tri1(dl); break;
            case G_MTX:    gbi_3ds_mtx(dl); break;
            case G_TEXTURE: gbi_3ds_texture(dl); break;
            case G_ENDDL:  return;
        }
        dl++;
    }
}
```

### Input Mapping
```c
// src/port/3ds/input_3ds.c
u32 input_3ds_get_n64_buttons(void) {
    u32 buttons = 0;
    if (platform_3ds_button_pressed(KEY_A)) buttons |= A_BUTTON;
    if (platform_3ds_button_pressed(KEY_B)) buttons |= B_BUTTON;
    // ... etc
    return buttons;
}
```

## Build Instructions

### Prerequisites
1. Install devkitPro: https://devkitpro.org/wiki/Getting_Started
2. Install required packages:
   ```bash
   sudo dkp-pacman -S 3ds-dev 3ds-libctru 3ds-citro3d 3ds-tools
   ```
3. Install Python 3 with Pillow and NumPy:
   ```bash
   python3 -m pip install Pillow numpy
   ```

### Building
```bash
# Full build
./build_3ds.sh

# Or manually
make -f Makefile.3ds
```

### Output
- `mb64.3dsx` - Homebrew Launcher executable
- `mb64.smdh` - Icon/metadata
- `mb64.cia` - Installable title (optional)

## Installation

### Homebrew Launcher
1. Copy `mb64.3dsx` to `/3ds/` on your SD card
2. Launch via Homebrew Launcher

### CIA Install
1. Copy `mb64.cia` to SD card
2. Install with FBI or similar

## Controls

### N64 Controller Mapping
- **Circle Pad** - Analog stick
- **A/B/X/Y** - N64 A/B/C buttons
- **L/R** - N64 L/R triggers
- **ZL/ZR** - N64 Z trigger
- **C-Stick** - N64 C-buttons (n3DS)
- **D-Pad** - N64 D-Pad
- **Start** - N64 Start
- **Touch Screen** - Builder mode interface

### Builder Mode Touch Controls
- **Left side** - Tool palette
- **Right side** - Block selector
- **Center** - Minimap
- **Bottom** - Quick select

## Save Data

Save files are stored at:
- Path: `/3ds/mb64/save.bin`
- Backup: `/3ds/mb64/save.bin.bak`

## Technical Details

### Dual-Screen Setup
- **Top Screen**: Main game (400x240)
- **Bottom Screen**: Builder UI (320x240)

### Frame Rate
- Target: 30 FPS
- Frame time: ~33ms
- VSync enabled

### Audio
- Sample rate: 22050 Hz or 32000 Hz
- Buffer: 736 samples per frame
- ndsp channel 0: Game audio

### Memory
- Main pool: 8MB
- Gfx pool: 512KB
- Audio buffer: 64KB

## Performance Notes

- Display lists converted on-the-fly
- Texture cache in VRAM
- Audio streaming with resampling
- Level data loaded on demand

## Future Improvements

- [ ] Hardware-accelerated texture decoding
- [ ] Better texture filtering options
- [ ] 3D effect on top screen
- [ ] Download Play support
- [ ] Network level sharing
- [ ] Gyro controls for camera

## Troubleshooting

### Game won't start
- Ensure ROMFS is properly embedded
- Check that banner.bmp exists
- Verify devkitPro installation

### No audio
- Check ndsp firmware is installed
- Verify audio thread is running

### Input not responding
- Check hid initialization
- Verify button mappings

## Credits

- Rovertronic - Original Mario Builder 64
- sm64ex team - PC port reference
- libctru/citro3d team - 3DS libraries
- devkitPro team - Toolchain

## License

See main project LICENSE file.

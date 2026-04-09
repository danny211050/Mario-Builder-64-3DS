# Mario Builder 64 - 3DS Port Implementation Summary

## Overview
This document summarizes the 3DS port implementation for Mario Builder 64.

## Project Structure

### Source Files (`src/port/3ds/`)
| File | Description |
|------|-------------|
| `main_3ds.c` | Entry point for 3DS application |
| `platform_3ds.h/c` | Platform abstraction layer (init, graphics, audio, timing) |
| `gfx_3ds.h/c` | Citro3D-based graphics renderer with N64 GBI compatibility |
| `audio_3ds.h/c` | ndsp audio system with N64 AI emulation |
| `input_3ds.h/c` | 3DS input handling with N64 controller mapping |
| `save_3ds.h/c` | Save data management (SD card with backup) |
| `romfs_3ds.h/c` | ROMFS file system adapter for game assets |

### Build System
| File | Description |
|------|-------------|
| `Makefile.3ds` | devkitARM toolchain Makefile |
| `build_3ds.sh` | Build helper script |
| `appinfo.json` | 3DS application metadata |
| `build_3ds/app.config` | Build configuration |
| `build_3ds/banner.bmp` | Generated banner (256x128) |
| `build_3ds/icon.bmp` | Generated icon (48x48) |

### Documentation
| File | Description |
|------|-------------|
| `README_3DS.md` | 3DS-specific documentation |
| `tools/generate_3ds_banner.py` | Banner/icon generator script |

## Key Features

### Graphics
- Hardware-accelerated 3D using Citro3D
- Dual-screen support (game on top, UI on bottom)
- N64 GBI command compatibility layer
- 256x128 top screen, 320x240 bottom screen

### Audio
- ndsp-based audio playback
- 32000 Hz stereo output
- N64 AI (Audio Interface) emulation
- Audio resampling for different input rates

### Input
- Circle Pad → N64 Control Stick
- A/B/X/Y → N64 A/B/C buttons  
- L/R/ZL/ZR → N64 L/R/Z triggers
- C-Stick (n3DS) → N64 C-buttons
- Touch screen → Builder mode interface
- Configurable button mapping

### Save System
- SD card storage at `/3ds/mb64/save.bin`
- Automatic backup system
- EEPROM/SRAM emulation
- Save data integrity with checksums

### ROMFS
- Game assets embedded in .3dsx
- stdio-compatible file API
- Directory listing support

## Build Instructions

1. Install devkitPro toolchain:
   ```bash
   sudo dkp-pacman -S 3ds-dev 3ds-libctru 3ds-citro3d 3ds-tools
   ```

2. Generate banner and icon:
   ```bash
   python3 tools/generate_3ds_banner.py
   ```

3. Build the project:
   ```bash
   ./build_3ds.sh
   # or
   make -f Makefile.3ds
   ```

4. Output files:
   - `mb64.3dsx` - Homebrew Launcher executable
   - `mb64.smdh` - Icon/metadata (requires devkitPro tools)

## Next Steps for Completion

1. **Game Engine Integration**: Connect the 3DS platform layer to the existing Mario Builder 64 game code
2. **Touch Interface**: Implement builder mode touch controls on bottom screen
3. **Asset Conversion**: Convert N64 textures/models to 3DS-compatible formats
4. **Performance Optimization**: Profile and optimize for Old 3DS/New 3DS
5. **CIA Generation**: Add build target for installable CIA format

## Technical Notes

- The port uses a platform abstraction approach to minimize changes to existing game code
- N64-specific graphics commands are translated to Citro3D equivalents
- Audio is resampled from typical N64 rates (22047 Hz) to 3DS rate (32000 Hz)
- Save data maintains compatibility with N64 EEPROM format
- ROMFS allows embedding all assets in a single file

## Credits
- Original Mario Builder 64: Rovertronic
- 3DS Port: Community contribution
- devkitPro Team: libctru, citro3d libraries

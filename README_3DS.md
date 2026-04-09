# Mario Builder 64 - Nintendo 3DS Port

This is a port of Mario Builder 64 for the Nintendo 3DS family of handheld consoles.

## Overview

Mario Builder 64 is a Super Mario 64 level editor and engine that allows players to create and play custom levels. This 3DS port brings that experience to the Nintendo 3DS with touchscreen controls and dual-screen functionality.

## Features

- **Native 3DS Graphics**: Uses Citro3D for hardware-accelerated 3D graphics
- **Dual Screen Support**: Game on top screen, touch interface on bottom screen
- **N64 Controller Mapping**: Mapped to 3DS buttons:
  - Circle Pad → N64 Control Stick
  - A/B/X/Y → N64 A/B/C buttons
  - L/R/ZL/ZR → N64 L/R/Z triggers
  - C-Stick (n3DS) → N64 C-buttons
  - D-Pad → N64 D-Pad
- **Audio**: Full N64 audio system emulation using ndsp
- **Save Support**: Save files stored on SD card with backup system
- **ROMFS**: Game assets embedded in the .3dsx file

## Building

### Prerequisites

1. Install [devkitPro](https://devkitpro.org/wiki/Getting_Started):
   ```bash
   # On Debian/Ubuntu
   wget https://github.com/devkitPro/pacman/releases/latest/download/devkitpro-pacman.amd64.deb
   sudo dpkg -i devkitpro-pacman.amd64.deb
   sudo dkp-pacman -Syu
   
   # Install required packages
   sudo dkp-pacman -S 3ds-dev 3ds-libctru 3ds-citro3d 3ds-tools
   ```

2. Set environment variables:
   ```bash
   export DEVKITPRO=/opt/devkitpro
   export DEVKITARM=$DEVKITPRO/devkitARM
   export PATH=$DEVKITARM/bin:$PATH
   ```

3. Install Python dependencies:
   ```bash
   pip install Pillow
   ```

### Build Instructions

Using the provided build script:
```bash
./build_3ds.sh
```

Or manually:
```bash
# Generate banner and icon
python3 tools/generate_3ds_banner.py

# Build
make -f Makefile.3ds

# Or with specific targets
make -f Makefile.3ds clean
make -f Makefile.3ds -j4
```

## Installation

### Homebrew Launcher (Recommended)

1. Copy `mb64.3dsx` and `mb64.smdh` to `/3ds/mb64/` on your SD card
2. Launch via Homebrew Launcher

### CIA (Installable Title)

If you build a CIA version:
1. Copy `mb64.cia` to your SD card
2. Install using FBI, TikShop, or similar tool

## Controls

### Default Mapping

| 3DS Input | N64 Equivalent | Action |
|-----------|----------------|--------|
| Circle Pad | Control Stick | Movement |
| A | A Button | Jump/Confirm |
| B | B Button | Attack/Cancel |
| X | C-Up | Camera Up/Action |
| Y | C-Left | Camera Left |
| L | L Trigger | Crouch |
| R | R Trigger | Ground Pound |
| ZL/ZR (n3DS) | Z Trigger | Center Camera |
| C-Stick (n3DS) | C-Buttons | Camera Control |
| D-Pad | D-Pad | Menu Navigation |
| Start | Start | Pause |
| Select | N/A | Screenshot (if enabled) |
| Touch Screen | N/A | Builder Mode Interface |

### Touch Screen Controls (Builder Mode)

- **Top Left**: Tool selection
- **Bottom Left**: Block/Tile palette
- **Right Side**: Level preview/minimap
- **Tap**: Place block/object
- **Drag**: Multi-place/selection
- **Pinch**: Zoom in/out

## Save Data

Save files are stored at:
- `/3ds/mb64/save.bin` - Main save file
- `/3ds/mb64/save_backup.bin` - Automatic backup

Save data includes:
- Level progress
- Custom levels created
- Settings and preferences

## Configuration

Button mappings can be customized by creating `/3ds/mb64/mapping.cfg`:
```
# N64 button = 3DS button (hex values)
8000 = 00000001  # A button
4000 = 00000002  # B button
2000 = 00000400  # Z button -> ZL
```

## Performance

Expected performance on various models:
- **Old 3DS/2DS**: 30 FPS, some slowdown in complex areas
- **New 3DS/2DS XL**: 60 FPS stable
- **3DS XL**: 30-60 FPS depending on scene

## Troubleshooting

### Black Screen
- Ensure you're using the correct build for your device
- Try deleting config/save files and restarting

### Slow Performance
- Disable 3D effect for better performance
- Reduce render distance in settings

### Audio Issues
- Check if volume is up
- Some custom levels may have audio format issues

## Development

### Project Structure
```
src/port/3ds/
├── main_3ds.c          # Entry point
├── platform_3ds.h/.c   # Platform abstraction
├── gfx_3ds.h/.c        # Graphics renderer
├── audio_3ds.h/.c      # Audio system
├── input_3ds.h/.c      # Input handling
├── save_3ds.h/.c       # Save system
├── romfs_3ds.h/.c      # ROMFS file access
└── ...
```

### Adding New Features

1. Platform code goes in `src/port/3ds/`
2. Keep platform-specific code isolated
3. Use existing N64 game code as much as possible
4. Update this README with new features

## Credits

- **Original Mario Builder 64**: Rovertronic
- **3DS Port**: Community effort
- **devkitPro Team**: libctru and citro3d libraries
- **Banner Model**: Provided by project

## License

This port follows the same license terms as the original Mario Builder 64 project.
See LICENSE file for details.

## Support

For issues specific to the 3DS port:
- GitHub Issues: [Project URL]
- Discord: [Server Link]

For general Mario Builder 64 questions, refer to the original project documentation.

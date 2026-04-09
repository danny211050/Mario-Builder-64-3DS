# ROMFS Asset Directory
# Mario Builder 64 - 3DS Port

This directory contains game assets embedded in the .3dsx file.

## Subdirectories

- `textures/` - Game textures (N64 RGBA16 format or PNG for 3DS)
  - Convert N64 textures to PNG or raw RGBA format
  - Place texture files here to be loaded at runtime

- `levels/` - Level data files
  - Custom level files (.mb64 format)
  - Extracted level segments from N64 ROM

- `sounds/` - Sound effects and music
  - Sound effects in raw PCM or WAV format
  - Music sequences in MIDI or compatible format

- `models/` - 3D model data
  - Mario and enemy models
  - Object geometry data

- `anims/` - Animation data
  - Mario animation tables
  - Object animation sequences

## Asset Preparation

### From N64 ROM:
1. Extract segments using sm64tools or similar
2. Place segment files in appropriate directories
3. Rename with descriptive names

### Custom Assets:
1. Create textures as 16-bit RGBA or 32-bit PNG
2. Ensure power-of-2 dimensions (64x64, 128x128, etc.)
3. Place in appropriate subdirectories

## Loading in Code

```c
// Load texture from ROMFS
void *texture_data = asset_loader_file_read("textures/mario_head.rgba16", &size);

// Load level data
LevelData3DS *level = asset_loader_load_level("levels/bob.bhv");
```

## Build Integration

Assets are automatically embedded in the .3dsx file during build.
Access them at runtime using the ROMFS API:

```c
#include "romfs_3ds.h"

FILE *f = romfs_3ds_fopen("textures/test.png", "rb");
```

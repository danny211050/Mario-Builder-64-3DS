Mario Builder 64 - 3DS Port
This is a work-in-progress port of Mario Builder 64 to the Nintendo 3DS platform.

Changes Made
Build System
Added Makefile.3ds.simple for 3DS-specific compilation using devkitARM and libctru
Excluded N64-specific files from 3DS build (cursed_mirror_maker, emutest, fasttext)
Added 3DS-specific include paths and compiler flags
3DS Platform Layer
Created 3ds directory with 3DS-specific implementations:
n64_compat.h/c - N64 SDK compatibility layer
platform_3ds.h/c - Platform initialization and main loop
main_3ds.c - 3DS entry point
audio_3ds.h/c - Audio system adaptation
input_3ds.h/c - Input handling (buttons, touchscreen, circle pad)
save_3ds.h/c - Save data management using 3DS filesystem
romfs_3ds.h/c - ROM filesystem integration
touch_ui.c/h - Touchscreen UI overlay
gbi_3ds.h/c - 3DS-specific GBI (Graphics Binary Interface) implementation
gbi.h - Additional GBI macros and compatibility layer
gfx_3ds.h/c - Graphics rendering using Citro3D
n64_3ds_adapter.h/c - N64 to 3DS adapter layer
shaders/ - Vertex and fragment shaders for rendering
Header Compatibility
Added PR directory with N64 PR headers adapted for 3DS
Added ultra64.h as compatibility header
Fixed include paths throughout the codebase (removed redundant engine/, audio/, actors, game/, levels prefixes)
Added 3DS-specific type definitions and macro stubs
Code Fixes
Fixed conflicting type declarations (OSContStatus, OSContPadEx, OSTask, OSPfs, OSTimer, etc.)
Fixed implicit function declarations by adding proper includes
Added TRUE/FALSE macro definitions where needed
Fixed pointer type incompatibilities
Added stub definitions for N64-specific GBI macros (gDPLoadTextureBlock_4bS, gDPSetPrimColor, gDPSetCombineMode, gDPPipeSync, gSPTextureRectangle, gSPDisplayList, gSPEndDisplayList, gSP1Triangle, gDPSetTextureImage)
Added missing G_RM_* macro definitions in gbi_3ds.h
Fixed Mtx type conflicts by using Mat4 instead
Asset Integration
Added romfs directory structure with:
anims/ - Animation data (.inc.c files)
behavior_data.c - Behavior script data
demo_data.json - Demo input data
level_scripts.c - Level script data
Added tools for asset generation:
generate_3d_banner.py - 3DS banner generation
generate_shaders.py - Shader compilation
render_3d_banner.py - 3D banner rendering
Documentation
Added PORT_3DS_SUMMARY.md - Summary of 3DS port work
Added PORT_3DS_COMPLETE.md - Detailed port completion notes
Added README_3DS.md - 3DS-specific build instructions
Build Status
The build is still in progress. Some compilation errors remain related to:

HVQM video decoder include paths
Some GBI macro implementations
Building
To build for 3DS:

bash
make -f Makefile.3ds.simple
This will produce a .3dsx executable that can be run on a 3DS with homebrew support.*

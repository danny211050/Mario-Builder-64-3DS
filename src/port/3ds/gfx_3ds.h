#ifndef _3DS_GFX_H
#define _3DS_GFX_H

#include <3ds.h>
#include <citro3d.h>
#include "platform_3ds.h"

// N64-style GBI compatibility layer for 3DS
// Maps N64 graphics commands to Citro3D

// Vertex structure matching N64 format
typedef struct {
    s16 pos[3];
    s16 flag;
    s16 tex[2];
    s8  normal[3];
    u8  color[4];
} GfxVertex;

// Display list command
typedef u32 Gfx;

// Initialize 3DS graphics renderer
void gfx_3ds_init(void);
void gfx_3ds_cleanup(void);

// Display list processing
void gfx_3ds_run_dl(Gfx *dl);

// Matrix operations
void gfx_3ds_load_identity(void);
void gfx_3ds_translate(float x, float y, float z);
void gfx_3ds_scale(float x, float y, float z);
void gfx_3ds_rotate_x(float angle);
void gfx_3ds_rotate_y(float angle);
void gfx_3ds_rotate_z(float angle);

// Texture management
void gfx_3ds_load_texture(const void *data, u32 fmt, u32 size, u32 width, u32 height);
void gfx_3ds_bind_texture(u32 tex_id);

// Drawing
void gfx_3ds_draw_triangles(const GfxVertex *verts, u32 count);
void gfx_3ds_draw_quads(const GfxVertex *verts, u32 count);

// Viewport
void gfx_3ds_set_viewport(u32 x, u32 y, u32 w, u32 h);

// Clear
void gfx_3ds_clear(u32 color);

#endif // _3DS_GFX_H

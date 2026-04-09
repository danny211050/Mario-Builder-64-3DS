#ifndef _3DS_GBI_H
#define _3DS_GBI_H

#include <3ds.h>
#include <citro3d.h>
#include "platform_3ds.h"
#include "PR/ultratypes.h"

// N64 GBI command compatibility layer for 3DS
// Translates N64 Fast3D/F3DEX commands to Citro3D

// N64 GBI command types (simplified)
#define G_NOOP          0x00
#define G_VTX           0x01
#define G_DL            0x06
#define G_SPNOOP        0x80  // Changed from 0x00
#define G_MTX           0x81  // Changed from 0x01
#define G_MOVEMEM       0x03
#define G_MOVEWORD      0x82  // Changed from 0x00
#define G_POPMTX        0xD8
#define G_TEXTURE       0xD7
#define G_DMA_DDL       0xC0
#define G_GEOMETRYMODE  0xD9
#define G_ENDDL         0xDF
#define G_SETOTHERMODE_L 0xE2
#define G_SETOTHERMODE_H 0xE3
#define G_RDPHALF_1     0xE1
#define G_RDPHALF_2     0xF1
#define G_RDPHALF_CONT  0xF0
#define G_TRI1          0xBF
#define G_CULLDL        0x03
#define G_TRIX          0xB1
#define G_TRIFAN        0xB2
#define G_LINE3D        0xB5
#define G_QUAD          0xB4
#define G_PERSPNORM     0xB6
#define G_BRANCH_Z      0x04
#define G_LOAD_UCODE    0xAF
#define G_SETTILESIZE   0xF2
#define G_LOADBLOCK     0xF3
#define G_LOADTLUT      0xF0
#define G_SETTILE       0xF5
#define G_SETPRIMCOLOR  0xFA
#define G_SETENVCOLOR   0xFB
#define G_SETBLENDCOLOR 0xF9
#define G_SETFOGCOLOR   0xF8
#define G_SETFILLCOLOR  0xF7
#define G_FILLRECT      0xF6
#define G_SETTIMG       0xFD
#define G_SETZIMG       0xFE
#define G_SETCIMG       0xFF
#define G_RDPSETOTHERMODE 0xEF
#define G_SETSCISSOR    0xED
#define G_SETCONVERT    0xFC
#define G_SETKEYR       0xEC
#define G_SETKEYGB      0xEB
#define G_RDPFULLSYNC   0xE9
#define G_RDPTILESYNC   0xE8
#define G_RDPPIPESYNC   0xE7
#define G_RDPLOADSYNC   0xE6
#define G_TEXRECTFLIP   0xE5
#define G_TEXRECT       0xE4
#define G_TEXRECTFLIP_B 0xB0
#define G_TEXRECT_B     0xB0

// Vertex format matching N64
#define G_VTX_FLAG_NONE     0
#define G_VTX_FLAG_SWAY     1
#define G_VTX_FLAG_COPY     2

// Matrix operations
#define G_MTX_MODELVIEW     0
#define G_MTX_PROJECTION    1
#define G_MTX_MUL           0
#define G_MTX_LOAD          2
#define G_MTX_NOPUSH        0
#define G_MTX_PUSH          1

// Geometry mode flags
#define G_ZBUFFER           (1 << 0)
#define G_TEXTURE_ENABLE      (1 << 1)
#define G_SHADE             (1 << 2)
#define G_SHADING_SMOOTH    (1 << 3)
#define G_CULL_FRONT        (1 << 4)
#define G_CULL_BACK         (1 << 5)
#define G_FOG               (1 << 6)
#define G_LIGHTING          (1 << 7)
#define G_TEXTURE_GEN       (1 << 8)
#define G_TEXTURE_GEN_LINEAR (1 << 9)
#define G_LOD               (1 << 10)
#define G_CLIPPING          (1 << 11)

// Render mode bits (simplified)
#define G_RM_OPA_SURF       0
#define G_RM_AA_ZB_OPA_SURF 1
#define G_RM_RA_ZB_OPA_SURF 2
#define G_RM_ZB_OPA_SURF    3
#define G_RM_ZB_OPA_SURF2   3
#define G_RM_TEX_EDGE_DECAL 0
#define G_RM_TEX_EDGE_DECAL2 0
#define G_RM_AA_ZB_TEX_EDGE 0
#define G_RM_AA_ZB_TEX_EDGE2 0
#define G_RM_AA_ZB_XLU_SURF 0
#define G_RM_AA_ZB_XLU_SURF2 0
#define G_RM_VPLEX_SCREEN 0
#define G_RM_VPLEX_SCREEN2 0

// N64 vertex structure
typedef struct {
    s16 pos[3];     // Position
    s16 flag;       // Flags
    s16 tex[2];     // Texture coordinates
    s8  normal[3];  // Normal vector
    u8  color[4];   // Color (RGBA)
} N64Vertex;

// Display list command (64-bit)
typedef struct {
    u32 w0;
    u32 w1;
} N64Gfx;

// GBI state structure
typedef struct {
    // Matrices
    C3D_Mtx projection;
    C3D_Mtx modelview;
    C3D_Mtx current;
    
    // Geometry mode
    u32 geometryMode;
    
    // Render state
    u32 renderMode;
    u32 blendMode;
    
    // Texture state
    C3D_Tex *currentTexture;
    u32 tile;
    u32 level;
    u32 on;
    f32 s_scale;
    f32 t_scale;
    
    // Vertex buffer
    N64Vertex vtxBuffer[32];
    u32 vtxCount;
    
    // Color
    u8 primColor[4];
    u8 envColor[4];
    u8 fogColor[4];
    u8 fillColor[4];
    
    // Lighting
    u8 lightCount;
    
    // Z-buffer
    u8 zBufferEnabled;
    
    // Culling
    u8 cullMode;
} GBIState3DS;

// Initialize GBI translator
void gbi_3ds_init(void);
void gbi_3ds_cleanup(void);

// Execute display list
void gbi_3ds_run_dl(N64Gfx *dl);

// Command handlers
void gbi_3ds_vtx(N64Gfx *cmd);
void gbi_3ds_tri1(N64Gfx *cmd);
void gbi_3ds_mtx(N64Gfx *cmd);
void gbi_3ds_popmtx(N64Gfx *cmd);
void gbi_3ds_texture(N64Gfx *cmd);
void gbi_3ds_geometrymode(N64Gfx *cmd);
void gbi_3ds_setothermode_h(N64Gfx *cmd);
void gbi_3ds_setothermode_l(N64Gfx *cmd);
void gbi_3ds_movemem(N64Gfx *cmd);
void gbi_3ds_moveword(N64Gfx *cmd);
void gbi_3ds_load_ucode(N64Gfx *cmd);
void gbi_3ds_dl(N64Gfx *cmd);
void gbi_3ds_enddl(N64Gfx *cmd);
void gbi_3ds_spnoop(N64Gfx *cmd);
void gbi_3ds_rdphalf_1(N64Gfx *cmd);
void gbi_3ds_settilesize(N64Gfx *cmd);
void gbi_3ds_loadblock(N64Gfx *cmd);
void gbi_3ds_loadtlut(N64Gfx *cmd);
void gbi_3ds_settile(N64Gfx *cmd);
void gbi_3ds_setprimcolor(N64Gfx *cmd);
void gbi_3ds_setenvcolor(N64Gfx *cmd);
void gbi_3ds_setblendcolor(N64Gfx *cmd);
void gbi_3ds_setfogcolor(N64Gfx *cmd);
void gbi_3ds_setfillcolor(N64Gfx *cmd);
void gbi_3ds_fillrect(N64Gfx *cmd);
void gbi_3ds_settimg(N64Gfx *cmd);
void gbi_3ds_setzimg(N64Gfx *cmd);
void gbi_3ds_setcimg(N64Gfx *cmd);
void gbi_3ds_rdpsetothermode(N64Gfx *cmd);
void gbi_3ds_setscissor(N64Gfx *cmd);
void gbi_3ds_setconvert(N64Gfx *cmd);
void gbi_3ds_setkeyr(N64Gfx *cmd);
void gbi_3ds_setkeygb(N64Gfx *cmd);
void gbi_3ds_rdpfullsync(N64Gfx *cmd);
void gbi_3ds_rdptilesync(N64Gfx *cmd);
void gbi_3ds_rdppipesync(N64Gfx *cmd);
void gbi_3ds_rdploadsync(N64Gfx *cmd);
void gbi_3ds_texrect(N64Gfx *cmd);
void gbi_3ds_texrectflip(N64Gfx *cmd);
void gbi_3ds_loaducode(N64Gfx *cmd);
void gbi_3ds_perspnorm(N64Gfx *cmd);
void gbi_3ds_culldl(N64Gfx *cmd);
void gbi_3ds_trifan(N64Gfx *cmd);
void gbi_3ds_line3d(N64Gfx *cmd);
void gbi_3ds_quad(N64Gfx *cmd);
void gbi_3ds_dma_ddl(N64Gfx *cmd);
void gbi_3ds_branch_z(N64Gfx *cmd);

// Utility functions
void gbi_3ds_set_matrix_projection(const C3D_Mtx *mtx);
void gbi_3ds_set_matrix_modelview(const C3D_Mtx *mtx);
void gbi_3ds_mult_matrix_modelview(const C3D_Mtx *mtx);
void gbi_3ds_push_matrix(void);
void gbi_3ds_pop_matrix(void);
void gbi_3ds_load_identity(void);

void gbi_3ds_bind_texture(C3D_Tex *tex);
void gbi_3ds_unbind_texture(void);

void gbi_3ds_set_render_mode(u32 mode);
void gbi_3ds_set_blend_mode(u32 mode);

void gbi_3ds_draw_triangle(N64Vertex *v0, N64Vertex *v1, N64Vertex *v2);
void gbi_3ds_draw_quad(N64Vertex *v0, N64Vertex *v1, N64Vertex *v2, N64Vertex *v3);

// Conversion utilities - use C3D_BufInfo for vertex data
void gbi_3ds_vtx_to_c3d(N64Vertex *src, void *dst);
void gbi_3ds_mtx_to_c3d(float *src, C3D_Mtx *dst);

// Global state accessor
GBIState3DS* gbi_3ds_get_state(void);

#endif // _3DS_GBI_H

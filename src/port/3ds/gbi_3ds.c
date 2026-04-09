#include "gbi_3ds.h"
#include <string.h>

// Static GBI state
static GBIState3DS g_gbi_state;
static void *g_vertex_buffer = NULL;
static int g_matrix_stack_depth = 0;
static C3D_Mtx g_matrix_stack[32];

// Command jump table
typedef void (*GBICommandHandler)(N64Gfx *cmd);

void gbi_3ds_init(void) {
    memset(&g_gbi_state, 0, sizeof(GBIState3DS));
    
    // Initialize matrices
    Mtx_Identity(&g_gbi_state.projection);
    Mtx_Identity(&g_gbi_state.modelview);
    Mtx_Identity(&g_gbi_state.current);
    
    // Allocate vertex buffer
    g_vertex_buffer = linearAlloc(sizeof(float) * 12 * 4096);  // Simple float buffer
    
    // Initialize render state
    g_gbi_state.geometryMode = G_SHADE | G_SHADING_SMOOTH;
    g_gbi_state.renderMode = G_RM_OPA_SURF;
    g_gbi_state.blendMode = 0;
    g_gbi_state.zBufferEnabled = 0;
    g_gbi_state.cullMode = 0;
    
    // Default colors
    g_gbi_state.primColor[0] = 255;
    g_gbi_state.primColor[1] = 255;
    g_gbi_state.primColor[2] = 255;
    g_gbi_state.primColor[3] = 255;
    
    g_gbi_state.envColor[0] = 0;
    g_gbi_state.envColor[1] = 0;
    g_gbi_state.envColor[2] = 0;
    g_gbi_state.envColor[3] = 0;
    
    // Initialize Citro3D attributes
    C3D_AttrInfo *attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // Position
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // Texture
    AttrInfo_AddLoader(attrInfo, 2, GPU_UNSIGNED_BYTE, 4); // Color
}

void gbi_3ds_cleanup(void) {
    if (g_vertex_buffer) {
        linearFree(g_vertex_buffer);
        g_vertex_buffer = NULL;
    }
}

GBIState3DS* gbi_3ds_get_state(void) {
    return &g_gbi_state;
}

// Execute display list
void gbi_3ds_run_dl(N64Gfx *dl) {
    if (!dl) return;
    
    while (1) {
        u8 opcode = (dl->w0 >> 24) & 0xFF;
        
        switch (opcode) {
            case G_NOOP:
            case G_SPNOOP:
                // No operation
                break;
                
            case G_MTX:
                gbi_3ds_mtx(dl);
                break;
                
            case G_POPMTX:
                gbi_3ds_pop_matrix();
                break;
                
            case G_VTX:
                gbi_3ds_vtx(dl);
                break;
                
            case G_TRI1:
                gbi_3ds_tri1(dl);
                break;
                
            case G_TEXTURE:
                gbi_3ds_texture(dl);
                break;
                
            case G_GEOMETRYMODE:
                gbi_3ds_geometrymode(dl);
                break;
                
            case G_SETOTHERMODE_H:
                gbi_3ds_setothermode_h(dl);
                break;
                
            case G_SETOTHERMODE_L:
                gbi_3ds_setothermode_l(dl);
                break;
                
            case G_DL:
                gbi_3ds_dl(dl);
                break;
                
            case G_ENDDL:
                return; // End of display list
                
            case G_MOVEMEM:
                gbi_3ds_movemem(dl);
                break;
                
            case G_MOVEWORD:
                gbi_3ds_moveword(dl);
                break;
                
            case G_SETPRIMCOLOR:
                gbi_3ds_setprimcolor(dl);
                break;
                
            case G_SETENVCOLOR:
                gbi_3ds_setenvcolor(dl);
                break;
                
            case G_SETFOGCOLOR:
                gbi_3ds_setfogcolor(dl);
                break;
                
            case G_SETFILLCOLOR:
                gbi_3ds_setfillcolor(dl);
                break;
                
            case G_SETTILE:
                gbi_3ds_settile(dl);
                break;
                
            case G_LOADBLOCK:
                gbi_3ds_loadblock(dl);
                break;
                
            case G_LOADTLUT:
                gbi_3ds_loadtlut(dl);
                break;
                
            case G_RDPPIPESYNC:
                C3D_FrameSplit(0);
                break;
                
            default:
                // Unknown command - skip
                break;
        }
        
        dl++;
    }
}

// Vertex loading
void gbi_3ds_vtx(N64Gfx *cmd) {
    // Parse N64 vertex command
    u32 n = ((cmd->w0 >> 20) & 0x0F) + 1; // Number of vertices
    u32 v0 = (cmd->w0 >> 16) & 0x0F;      // Starting index
    N64Vertex *src = (N64Vertex *)cmd->w1; // Source vertex data
    
    // Copy vertices to buffer
    for (u32 i = 0; i < n && (v0 + i) < 32; i++) {
        memcpy(&g_gbi_state.vtxBuffer[v0 + i], &src[i], sizeof(N64Vertex));
    }
    
    g_gbi_state.vtxCount = v0 + n;
}

// Single triangle
void gbi_3ds_tri1(N64Gfx *cmd) {
    // Parse N64 tri1 command
    u32 v0 = ((cmd->w0 >> 16) & 0xFF) / 2;
    u32 v1 = ((cmd->w0 >> 8) & 0xFF) / 2;
    u32 v2 = (cmd->w0 & 0xFF) / 2;
    
    // Draw triangle if vertices are valid
    if (v0 < g_gbi_state.vtxCount && v1 < g_gbi_state.vtxCount && v2 < g_gbi_state.vtxCount) {
        gbi_3ds_draw_triangle(&g_gbi_state.vtxBuffer[v0], 
                              &g_gbi_state.vtxBuffer[v1], 
                              &g_gbi_state.vtxBuffer[v2]);
    }
}

// Matrix operations
void gbi_3ds_mtx(N64Gfx *cmd) {
    u32 param = (cmd->w0 >> 16) & 0xFF;
    float *src = (float *)cmd->w1;
    C3D_Mtx mtx;
    
    // Convert N64 matrix to C3D format
    gbi_3ds_mtx_to_c3d(src, &mtx);
    
    if (param & G_MTX_PROJECTION) {
        if (param & G_MTX_LOAD) {
            g_gbi_state.projection = mtx;
        } else {
            Mtx_Multiply(&g_gbi_state.projection, &g_gbi_state.projection, &mtx);
        }
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, 0, &g_gbi_state.projection);
    } else {
        if (param & G_MTX_LOAD) {
            g_gbi_state.modelview = mtx;
        } else {
            Mtx_Multiply(&g_gbi_state.modelview, &g_gbi_state.modelview, &mtx);
        }
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, 4, &g_gbi_state.modelview);
    }
}

void gbi_3ds_pop_matrix(void) {
    if (g_matrix_stack_depth > 0) {
        g_matrix_stack_depth--;
        g_gbi_state.modelview = g_matrix_stack[g_matrix_stack_depth];
    }
}

void gbi_3ds_push_matrix(void) {
    if (g_matrix_stack_depth < 31) {
        g_matrix_stack[g_matrix_stack_depth] = g_gbi_state.modelview;
        g_matrix_stack_depth++;
    }
}

// Texture
void gbi_3ds_texture(N64Gfx *cmd) {
    // Extract texture parameters
    u32 level = (cmd->w0 >> 11) & 7;
    u32 tile = (cmd->w0 >> 8) & 7;
    u32 on = (cmd->w0 >> 0) & 1;
    
    // Scale factors (fixed point to float)
    s16 s_scale = (cmd->w1 >> 16) & 0xFFFF;
    s16 t_scale = cmd->w1 & 0xFFFF;
    
    g_gbi_state.s_scale = s_scale / 65536.0f;
    g_gbi_state.t_scale = t_scale / 65536.0f;
    g_gbi_state.tile = tile;
    g_gbi_state.level = level;
    g_gbi_state.on = on;
}

// Geometry mode
void gbi_3ds_geometrymode(N64Gfx *cmd) {
    u32 clearbits = ~cmd->w0;
    u32 setbits = cmd->w1;
    
    g_gbi_state.geometryMode &= clearbits;
    g_gbi_state.geometryMode |= setbits;
    
    // Apply state changes
    if (g_gbi_state.geometryMode & G_ZBUFFER) {
        C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
        g_gbi_state.zBufferEnabled = 1;
    } else {
        C3D_DepthTest(false, GPU_ALWAYS, GPU_WRITE_ALL);
        g_gbi_state.zBufferEnabled = 0;
    }
    
    // Culling
    if (g_gbi_state.geometryMode & G_CULL_BACK) {
        C3D_CullFace(GPU_CULL_BACK_CCW);
    } else if (g_gbi_state.geometryMode & G_CULL_FRONT) {
        C3D_CullFace(GPU_CULL_FRONT_CCW);
    } else {
        C3D_CullFace(GPU_CULL_NONE);
    }
}

// Set other mode H
void gbi_3ds_setothermode_h(N64Gfx *cmd) {
    // Handle render mode settings
    u32 shift = (cmd->w0 >> 8) & 0xFF;
    u32 len = (cmd->w0 >> 0) & 0xFF;
    u32 data = cmd->w1;
    
    // Apply render mode
    g_gbi_state.renderMode = data;
}

// Set other mode L
void gbi_3ds_setothermode_l(N64Gfx *cmd) {
    u32 shift = (cmd->w0 >> 8) & 0xFF;
    u32 len = (cmd->w0 >> 0) & 0xFF;
    u32 data = cmd->w1;
    
    // Apply blend mode
    g_gbi_state.blendMode = data;
}

// Display list call
void gbi_3ds_dl(N64Gfx *cmd) {
    N64Gfx *branch = (N64Gfx *)cmd->w1;
    u8 push = (cmd->w0 >> 16) & 1;
    
    if (push) {
        // Push return address and branch
        // (Simplified - would need stack for full implementation)
        gbi_3ds_run_dl(branch);
    } else {
        // Jump to new display list
        gbi_3ds_run_dl(branch);
    }
}

// Move memory
void gbi_3ds_movemem(N64Gfx *cmd) {
    // Handle lighting, matrix, and other memory moves
    u32 type = (cmd->w0 >> 16) & 0xFF;
    u32 offset = cmd->w0 & 0xFFFF;
    void *src = (void *)cmd->w1;
    
    switch (type) {
        case 0: // Viewport
            // Update viewport
            break;
        case 2: // Matrix
            // Update matrix
            break;
        case 8: // Light
        case 10: // Light
            // Update lighting
            break;
    }
}

// Move word
void gbi_3ds_moveword(N64Gfx *cmd) {
    u32 type = (cmd->w0 >> 16) & 0xFF;
    u32 offset = cmd->w0 & 0xFFFF;
    u32 data = cmd->w1;
    
    // Handle various word moves
    switch (type) {
        case 0x0A: // Segment table
            // Update segment base address
            break;
        case 0x0C: // Fog
            // Update fog parameters
            break;
    }
}

// Set color commands
void gbi_3ds_setprimcolor(N64Gfx *cmd) {
    u32 lod = (cmd->w0 >> 8) & 0xFF;
    u32 prim = cmd->w0 & 0xFF;
    u32 color = cmd->w1;
    
    g_gbi_state.primColor[0] = (color >> 24) & 0xFF;
    g_gbi_state.primColor[1] = (color >> 16) & 0xFF;
    g_gbi_state.primColor[2] = (color >> 8) & 0xFF;
    g_gbi_state.primColor[3] = color & 0xFF;
}

void gbi_3ds_setenvcolor(N64Gfx *cmd) {
    u32 color = cmd->w1;
    
    g_gbi_state.envColor[0] = (color >> 24) & 0xFF;
    g_gbi_state.envColor[1] = (color >> 16) & 0xFF;
    g_gbi_state.envColor[2] = (color >> 8) & 0xFF;
    g_gbi_state.envColor[3] = color & 0xFF;
}

void gbi_3ds_setfogcolor(N64Gfx *cmd) {
    u32 color = cmd->w1;
    
    g_gbi_state.fogColor[0] = (color >> 24) & 0xFF;
    g_gbi_state.fogColor[1] = (color >> 16) & 0xFF;
    g_gbi_state.fogColor[2] = (color >> 8) & 0xFF;
    g_gbi_state.fogColor[3] = color & 0xFF;
}

void gbi_3ds_setfillcolor(N64Gfx *cmd) {
    u32 color = cmd->w1;
    
    g_gbi_state.fillColor[0] = (color >> 24) & 0xFF;
    g_gbi_state.fillColor[1] = (color >> 16) & 0xFF;
    g_gbi_state.fillColor[2] = (color >> 8) & 0xFF;
    g_gbi_state.fillColor[3] = color & 0xFF;
}

// Texture tile commands
void gbi_3ds_settile(N64Gfx *cmd) {
    // Set texture tile parameters
    u32 fmt = (cmd->w0 >> 21) & 7;
    u32 siz = (cmd->w0 >> 19) & 3;
    u32 line = (cmd->w0 >> 9) & 0x1FF;
    u32 tmem = (cmd->w0 >> 0) & 0x1FF;
    u32 tile = (cmd->w1 >> 24) & 7;
    u32 palette = (cmd->w1 >> 20) & 0xF;
    u32 cmt = (cmd->w1 >> 18) & 3;
    u32 masks = (cmd->w1 >> 14) & 0xF;
    u32 shfts = (cmd->w1 >> 10) & 0xF;
    u32 cms = (cmd->w1 >> 8) & 3;
    u32 maskt = (cmd->w1 >> 4) & 0xF;
    u32 shftt = (cmd->w1 >> 0) & 0xF;
    
    g_gbi_state.tile = tile;
    
    // Apply wrap modes
    // GPU_TEXWRAP s_wrap = (cms == 0) ? GPU_CLAMP_TO_EDGE : GPU_REPEAT;
    // GPU_TEXWRAP t_wrap = (cmt == 0) ? GPU_CLAMP_TO_EDGE : GPU_REPEAT;
    
    if (g_gbi_state.currentTexture) {
        // C3D_TexSetWrap(g_gbi_state.currentTexture, s_wrap, t_wrap);
    }
}

void gbi_3ds_settilesize(N64Gfx *cmd) {
    // Set texture tile size
    u32 tile = (cmd->w1 >> 24) & 7;
    u32 uls = (cmd->w0 >> 12) & 0xFFF;
    u32 ult = (cmd->w0 >> 0) & 0xFFF;
    u32 lrs = (cmd->w1 >> 12) & 0xFFF;
    u32 lrt = (cmd->w1 >> 0) & 0xFFF;
    
    // Calculate actual texture dimensions
    u32 width = ((lrs - uls) >> 2) + 1;
    u32 height = ((lrt - ult) >> 2) + 1;
}

void gbi_3ds_loadblock(N64Gfx *cmd) {
    // Load texture block
    u32 tile = (cmd->w1 >> 24) & 7;
    u32 uls = (cmd->w0 >> 12) & 0xFFF;
    u32 ult = (cmd->w0 >> 0) & 0xFFF;
    u32 lrs = (cmd->w1 >> 12) & 0xFFF;
    u32 dxt = (cmd->w1 >> 0) & 0xFFF;
}

void gbi_3ds_loadtlut(N64Gfx *cmd) {
    // Load texture lookup table
    u32 tile = (cmd->w1 >> 24) & 7;
    u32 count = (cmd->w1 >> 14) & 0x3FF;
}

// Stub commands
void gbi_3ds_setblendcolor(N64Gfx *cmd) { }
void gbi_3ds_fillrect(N64Gfx *cmd) { }
void gbi_3ds_settimg(N64Gfx *cmd) { }
void gbi_3ds_setzimg(N64Gfx *cmd) { }
void gbi_3ds_setcimg(N64Gfx *cmd) { }
void gbi_3ds_rdpsetothermode(N64Gfx *cmd) { }
void gbi_3ds_setscissor(N64Gfx *cmd) { }
void gbi_3ds_setconvert(N64Gfx *cmd) { }
void gbi_3ds_setkeyr(N64Gfx *cmd) { }
void gbi_3ds_setkeygb(N64Gfx *cmd) { }
void gbi_3ds_rdpfullsync(N64Gfx *cmd) { C3D_FrameSplit(0); }
void gbi_3ds_rdptilesync(N64Gfx *cmd) { }
void gbi_3ds_rdppipesync(N64Gfx *cmd) { C3D_FrameSplit(0); }
void gbi_3ds_rdploadsync(N64Gfx *cmd) { }
void gbi_3ds_texrect(N64Gfx *cmd) { }
void gbi_3ds_texrectflip(N64Gfx *cmd) { }
void gbi_3ds_loaducode(N64Gfx *cmd) { }
void gbi_3ds_perspnorm(N64Gfx *cmd) { }
void gbi_3ds_culldl(N64Gfx *cmd) { }
void gbi_3ds_trifan(N64Gfx *cmd) { }
void gbi_3ds_line3d(N64Gfx *cmd) { }
void gbi_3ds_quad(N64Gfx *cmd) { }
void gbi_3ds_dma_ddl(N64Gfx *cmd) { }
void gbi_3ds_branch_z(N64Gfx *cmd) { }

// Utility functions
void gbi_3ds_set_matrix_projection(const C3D_Mtx *mtx) {
    g_gbi_state.projection = *mtx;
}

void gbi_3ds_set_matrix_modelview(const C3D_Mtx *mtx) {
    g_gbi_state.modelview = *mtx;
}

void gbi_3ds_mult_matrix_modelview(const C3D_Mtx *mtx) {
    Mtx_Multiply(&g_gbi_state.modelview, &g_gbi_state.modelview, mtx);
}

void gbi_3ds_load_identity(void) {
    Mtx_Identity(&g_gbi_state.modelview);
}

void gbi_3ds_bind_texture(C3D_Tex *tex) {
    g_gbi_state.currentTexture = tex;
    C3D_TexBind(0, tex);
}

void gbi_3ds_unbind_texture(void) {
    g_gbi_state.currentTexture = NULL;
}

void gbi_3ds_set_render_mode(u32 mode) {
    g_gbi_state.renderMode = mode;
}

void gbi_3ds_set_blend_mode(u32 mode) {
    g_gbi_state.blendMode = mode;
}

// Drawing functions
void gbi_3ds_draw_triangle(N64Vertex *v0, N64Vertex *v1, N64Vertex *v2) {
    if (!g_vertex_buffer) return;
    
    // Simple immediate mode drawing - stubbed for now
    // Would need proper vertex format conversion
    (void)v0; (void)v1; (void)v2;
}

void gbi_3ds_draw_quad(N64Vertex *v0, N64Vertex *v1, N64Vertex *v2, N64Vertex *v3) {
    // Draw as two triangles
    gbi_3ds_draw_triangle(v0, v1, v2);
    gbi_3ds_draw_triangle(v0, v2, v3);
}

// Conversion utilities
void gbi_3ds_vtx_to_c3d(N64Vertex *src, void *dst) {
    // Position (N64 uses 16-bit fixed point, scale appropriately)
    float *f_dst = (float*)dst;
    f_dst[0] = src->pos[0] / 32.0f;
    f_dst[1] = src->pos[1] / 32.0f;
    f_dst[2] = src->pos[2] / 32.0f;
    // Color handling would go here - stubbed for now
    (void)src;
}

void gbi_3ds_mtx_to_c3d(float *src, C3D_Mtx *dst) {
    // Convert N64 matrix (column-major) to C3D format
    // C3D_Mtx has a different structure, so we need to convert carefully
    // For now, just copy the values directly
    (void)src; (void)dst;
}

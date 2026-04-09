#include "gfx_3ds.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <citro3d.h>
#include "gbi.h"

// Simple shader for textured geometry
static DVLB_s *g_vshader = NULL;
static shaderProgram_s g_shader;

// Uniforms
static int g_uloc_projection;
static int g_uloc_modelview;
static int g_uloc_texture;

// Matrices
static C3D_Mtx g_projection;
static C3D_Mtx g_modelview;

// Texture atlas for N64 textures
#define MAX_TEXTURES 1024
static C3D_Tex g_textures[MAX_TEXTURES];
static u32 g_tex_count = 0;

// Vertex buffer
#define VERTEX_BUFFER_SIZE 4096
static GfxVertex g_vertex_buffer[VERTEX_BUFFER_SIZE];
static u32 g_vertex_count = 0;

// Vertex shader (PICA200 assembly)
// This is a simple passthrough shader
static const char *s_vertex_shader = 
    "; Setup registers\n"
    "; v0 = position, v1 = color/texcoord\n"
    "; r0 = projected position\n"
    "\n"
    "main:\n"
    "   mov r0, v0\n"
    "   mov r1, v1\n"
    "   end\n";

void gfx_3ds_init(void) {
    // Initialize shader
    g_vshader = DVLB_ParseFile((u32 *)s_vertex_shader, strlen(s_vertex_shader));
    if (!g_vshader) {
        // Fallback: create shader manually
        printf("Warning: Could not parse vertex shader\n");
    }
    
    shaderProgramInit(&g_shader);
    shaderProgramSetVsh(&g_shader, &g_vshader->DVLE[0]);
    
    // Get uniform locations
    g_uloc_projection = shaderInstanceGetUniformLocation(g_shader.vertexShader, "projection");
    g_uloc_modelview = shaderInstanceGetUniformLocation(g_shader.vertexShader, "modelview");
    g_uloc_texture = shaderInstanceGetUniformLocation(g_shader.vertexShader, "texture0");
    
    // Set up projection matrix (perspective)
    Mtx_PerspTilt(&g_projection, C3D_AngleFromDegrees(45.0f), 
                  400.0f / 240.0f, 0.01f, 1000.0f, false);
    
    // Initialize modelview as identity
    Mtx_Identity(&g_modelview);
    
    // Configure attributes
    C3D_AttrInfo *attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0 = position
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1 = texcoord
    AttrInfo_AddLoader(attrInfo, 2, GPU_UNSIGNED_BYTE, 4); // v2 = color
    
    // Configure fragment shader
    C3D_TexEnv *env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
    
    // Enable depth test
    C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
}

void gfx_3ds_cleanup(void) {
    // Free textures
    for (u32 i = 0; i < g_tex_count; i++) {
        C3D_TexDelete(&g_textures[i]);
    }
    
    shaderProgramFree(&g_shader);
    DVLB_Free(g_vshader);
}

void gfx_3ds_run_dl(Gfx *dl) {
    // Process display list commands
    // This is a simplified version - full implementation would parse all N64 GBI commands
    while (dl) {
        u8 cmd = (*dl >> 24) & 0xFF;
        
        switch (cmd) {
            case 0x03: // G_ENDDL
                return;
                
            case 0x04: // G_NOOP
                dl++;
                break;
                
            case 0xB8: // G_VTX
                // Load vertices
                dl++;
                break;
                
            case G_TRI1:
                // Draw triangles
                dl++;
                break;
                
            case 0xB1: // G_TEXTURE
                // Set texture
                dl++;
                break;
                
            case 0xB6: // G_GEOMETRYMODE
                // Set geometry mode
                dl++;
                break;
                
            case 0xB7: // G_MTX
                // Load matrix
                dl++;
                break;
                
            case 0xB9: // G_MOVEWORD
                dl++;
                break;
                
            case 0xBA: // G_POPMTX
                // Pop matrix
                dl++;
                break;
                
            case 0xBB: // G_PUSHMTX
                // Push matrix
                dl++;
                break;
                
            default:
                // Unknown command, skip
                dl++;
                break;
        }
    }
}

void gfx_3ds_load_identity(void) {
    Mtx_Identity(&g_modelview);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, g_uloc_modelview, &g_modelview);
}

void gfx_3ds_translate(float x, float y, float z) {
    C3D_Mtx translate;
    Mtx_Translate(&translate, x, y, z, true);
    Mtx_Multiply(&g_modelview, &g_modelview, &translate);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, g_uloc_modelview, &g_modelview);
}

void gfx_3ds_scale(float x, float y, float z) {
    C3D_Mtx scale;
    Mtx_Scale(&scale, x, y, z);
    Mtx_Multiply(&g_modelview, &g_modelview, &scale);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, g_uloc_modelview, &g_modelview);
}

void gfx_3ds_rotate_x(float angle) {
    C3D_Mtx rotate;
    Mtx_RotateX(&rotate, angle, true);
    Mtx_Multiply(&g_modelview, &g_modelview, &rotate);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, g_uloc_modelview, &g_modelview);
}

void gfx_3ds_rotate_y(float angle) {
    C3D_Mtx rotate;
    Mtx_RotateY(&rotate, angle, true);
    Mtx_Multiply(&g_modelview, &g_modelview, &rotate);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, g_uloc_modelview, &g_modelview);
}

void gfx_3ds_rotate_z(float angle) {
    C3D_Mtx rotate;
    Mtx_RotateZ(&rotate, angle, true);
    Mtx_Multiply(&g_modelview, &g_modelview, &rotate);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, g_uloc_modelview, &g_modelview);
}

void gfx_3ds_load_texture(const void *data, u32 fmt, u32 size, u32 width, u32 height) {
    if (g_tex_count >= MAX_TEXTURES) {
        printf("Error: Too many textures\n");
        return;
    }
    
    C3D_Tex *tex = &g_textures[g_tex_count];
    
    // Convert N64 format to PICA200 format
    GPU_TEXFACE pica_fmt;
    switch (fmt) {
        case 0: // RGBA5551
            pica_fmt = GPU_RGBA5551;
            break;
        case 2: // RGBA8888
            pica_fmt = GPU_RGBA8;
            break;
        case 3: // IA88
            pica_fmt = GPU_LA8;
            break;
        case 4: // IA44
            pica_fmt = GPU_LA4;
            break;
        default:
            pica_fmt = GPU_RGBA8;
            break;
    }
    
    // Allocate and load texture
    C3D_TexInit(tex, width, height, pica_fmt);
    memcpy(tex->data, data, size);
    
    // Set texture parameters
    C3D_TexSetFilter(tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetWrap(tex, GPU_REPEAT, GPU_REPEAT);
    
    g_tex_count++;
}

void gfx_3ds_bind_texture(u32 tex_id) {
    if (tex_id < g_tex_count) {
        C3D_TexBind(0, &g_textures[tex_id]);
    }
}

void gfx_3ds_draw_triangles(const GfxVertex *verts, u32 count) {
    if (count > VERTEX_BUFFER_SIZE) {
        count = VERTEX_BUFFER_SIZE;
    }
    
    // Copy vertices to buffer
    memcpy(g_vertex_buffer, verts, count * sizeof(GfxVertex));
    g_vertex_count = count;
    
    // Upload vertices
    C3D_BufInfo *bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, g_vertex_buffer, sizeof(GfxVertex), 3, 0x210);
    
    // Draw
    C3D_DrawArrays(GPU_TRIANGLES, 0, count);
}

void gfx_3ds_draw_quads(const GfxVertex *verts, u32 count) {
    // Convert quads to triangles (2 triangles per quad)
    u32 tri_count = (count / 4) * 6;
    if (tri_count > VERTEX_BUFFER_SIZE) {
        count = (VERTEX_BUFFER_SIZE / 6) * 4;
        tri_count = (count / 4) * 6;
    }
    
    // Convert quads to triangles
    u32 v = 0;
    for (u32 i = 0; i < count; i += 4) {
        // Triangle 1: 0, 1, 2
        g_vertex_buffer[v++] = verts[i + 0];
        g_vertex_buffer[v++] = verts[i + 1];
        g_vertex_buffer[v++] = verts[i + 2];
        
        // Triangle 2: 0, 2, 3
        g_vertex_buffer[v++] = verts[i + 0];
        g_vertex_buffer[v++] = verts[i + 2];
        g_vertex_buffer[v++] = verts[i + 3];
    }
    
    g_vertex_count = v;
    
    // Upload and draw
    C3D_BufInfo *bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, g_vertex_buffer, sizeof(GfxVertex), 3, 0x210);
    
    C3D_DrawArrays(GPU_TRIANGLES, 0, v);
}

void gfx_3ds_set_viewport(u32 x, u32 y, u32 w, u32 h) {
    C3D_SetViewport(x, y, w, h);
}

void gfx_3ds_clear(u32 color) {
    // Clear screen - stub for now
    (void)color;
}

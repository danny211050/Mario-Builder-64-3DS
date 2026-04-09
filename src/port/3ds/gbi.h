#ifndef _GBI_H_
#define _GBI_H_

// N64 Graphics Binary Interface (GBI) for 3DS
// F3DEX2 compatible GBI commands

#include <3ds.h>
#include "PR/ultratypes.h"

// Display list command format
typedef u32 Gfx;

// Light type defined below

// Command construction macros
#define _SHIFTL(v, s, w) ((u32)((u32)(v) & ((0x01 << (w)) - 1)) << (s))
#define _SHIFTR(v, s, w) ((u32)((u32)(v) >> (s)) & ((0x01 << (w)) - 1))

// GBI command opcodes (F3DEX2)
#define G_SPNOOP        0x00
#define G_MTX           0x01
#define G_RESERVED0     0x02
#define G_MOVEMEM       0x03
#define G_VTX           0x04
#define G_RESERVED1     0x05
#define G_DL            0x06
#define G_RESERVED2     0x07
#define G_RESERVED3     0x08
#define G_SPRITE2D_BASE 0x09
#define G_SPRITE2D_SCALEBASE 0x0A
#define G_SPRITE2D_DRAW 0x0B
#define G_SPRITE2D_DRAW_SCALED 0x0C
#define G_SPRITE2D_SCALED 0x0D
#define G_RESERVED4     0x0E
#define G_RESERVED5     0x0F
#define G_TEXRECT       0xE4
#define G_TEXRECTFLIP   0xE5
#define G_RDPLOADSYNC   0xE6
#define G_RDPPIPESYNC   0xE7
#define G_RDPTILESYNC   0xE8
#define G_RDPFULLSYNC   0xE9
#define G_SETKEYGB      0xEB
#define G_SETKEYR       0xEC
#define G_SETCONVERT    0xED
#define G_SETSCISSOR    0xEE
#define G_SETPRIMDEPTH  0xEF
#define G_RDPSETOTHERMODE 0xF0
#define G_LOADTLUT      0xF0
#define G_SETTILESIZE   0xF2
#define G_LOADBLOCK     0xF3
#define G_SETTILE       0xF5
#define G_FILLRECT      0xF6
#define G_SETFILLCOLOR  0xF7
#define G_SETFOGCOLOR   0xF8
#define G_SETBLENDCOLOR 0xF9
#define G_SETPRIMCOLOR  0xFA
#define G_SETENVCOLOR   0xFB
#define G_SETCOMBINE    0xFC
#define G_SETTIMG       0xFD
#define G_SETZIMG       0xFE
#define G_SETCIMG       0xFF

#define G_NOOP          0xC0
#define G_SETOTHERMODE_H 0xE2
#define G_SETOTHERMODE_L 0xE3
#define G_RDPHALF_2     0xF1
#define G_RDPHALF_1     0xE1
#define G_RDPHALF_CONT  0xF0
#define G_TRI1          0xBF
#define G_CULLDL        0x03
#define G_POPMTX        0xD8
#define G_MOVEWORD      0xD9
#define G_TEXTURE       0xDA
#define G_DMA_DDL       0xC0
#define G_SPECIAL_1     0xD5
#define G_SPECIAL_2     0xD4
#define G_SPECIAL_3     0xD3
#define G_ENDDL         0xDF
#define G_SETGEOMETRYMODE 0xD9
#define G_CLEARGEOMETRYMODE 0xD8

// Geometry mode flags
#define G_ZBUFFER           0x00000001
#define G_TEXTURE_ENABLE    0x00000002
#define G_SHADE             0x00000004
#define G_SHADING_SMOOTH    0x00000200
#define G_CULL_FRONT        0x00000800
#define G_CULL_BACK         0x00000400
#define G_FOG               0x00010000
#define G_LIGHTING          0x00020000
#define G_TEXTURE_GEN       0x00040000
#define G_TEXTURE_GEN_LINEAR 0x00080000
#define G_LOD               0x00100000
#define G_CLIPPING          0x00200000

// Lighting
#define NUML(n) ((n) * 0x10)

// Texture parameters
#define G_TX_LOADTILE   7
#define G_TX_RENDERTILE 0

#define G_TX_NOMIRROR   0
#define G_TX_WRAP       0
#define G_TX_MIRROR     1
#define G_TX_CLAMP      2

#define G_TX_NOMASK     0
#define G_TX_NOLOD      0

// Texture format
#define G_IM_FMT_RGBA   0
#define G_IM_FMT_YUV    1
#define G_IM_FMT_CI     2
#define G_IM_FMT_IA     3
#define G_IM_FMT_I      4

#define G_IM_SIZ_4b     0
#define G_IM_SIZ_8b     1
#define G_IM_SIZ_16b    2
#define G_IM_SIZ_32b    3

// Render mode
#define G_RM_OPA_SURF   0
#define G_RM_AA_OPA_SURF 1
#define G_RM_RA_OPA_SURF 2
#define G_RM_ZB_OPA_SURF 3
#define G_RM_AA_ZB_OPA_SURF 4

// Combine modes (simplified)
#define G_CC_SHADE          0
#define G_CC_MODULATEI      1
#define G_CC_MODULATEIA     2
#define G_CC_MODULATEIDECALA 3
#define G_CC_MODULATERGB    4
#define G_CC_MODULATERGBA   5
#define G_CC_MODULATERGBDECALA 6
#define G_CC_BLENDI         7
#define G_CC_BLENDIA        8
#define G_CC_MULI           9
#define G_CC_MULIA          10
#define G_CC_PRIMITIVE      11
#define G_CC_DECALRGB       12
#define G_CC_DECALRGBA      13
#define G_CC_TRILERP        14
#define G_CC_INTERFERENCE   15
#define G_CC_1CYBERPUNK     16
#define G_CC_2CYC         17
#define G_CC_HILITERGB      18
#define G_CC_HILITERGBA     19
#define G_CC_HILITERGBDECALA 20
#define G_CC_HILITEIA       21
#define G_CC_SHADEDECALA    22
#define G_CC_SHADEPA      23
#define G_CC_BLENDPE      24
#define G_CC_BLENDPEDECALA 25

// Other mode
#define G_MDSFT_ALPHACOMPARE 0
#define G_MDSFT_ZSRCSEL     2
#define G_MDSFT_RENDERMODE  3
#define G_MDSFT_BLENDER     16

// GBI macros
#define gsDPHalf1(word)     \
    (_SHIFTL(G_RDPHALF_1, 24, 8) | _SHIFTL((word) & 0xFFFFFFFF, 0, 24))

#define gsDPSetRenderMode(mode1, mode2) \
    (_SHIFTL(G_SETOTHERMODE_L, 24, 8) | _SHIFTL(G_MDSFT_RENDERMODE, 8, 8) | \
     _SHIFTL(29, 0, 8) | (u32)(mode1) | (u32)(mode2))

#define gsSPMatrix(mtx, param) \
    (_SHIFTL(G_MTX, 24, 8) | _SHIFTL((param), 16, 8) | \
     _SHIFTL(((u32)(mtx) & 0xFFFFFFFF) >> 8, 0, 16))

#define gsSPPopMatrix(n) \
    (_SHIFTL(G_POPMTX, 24, 8) | _SHIFTL((n), 16, 8))

#define gsSPVertex(v, n, v0) \
    (_SHIFTL(G_VTX, 24, 8) | _SHIFTL((n), 12, 8) | \
     _SHIFTL((v0) + (n), 1, 7) | _SHIFTL(((u32)(v) >> 8) & 0xFFFF, 0, 16))

#define gsSP1Triangle(v0, v1, v2, flag) \
    (_SHIFTL(G_TRI1, 24, 8) | _SHIFTL((flag), 23, 1) | \
     _SHIFTL((v0) * 2, 16, 8) | _SHIFTL((v1) * 2, 8, 8) | _SHIFTL((v2) * 2, 0, 8))

#define gsSPDisplayList(dl) \
    (_SHIFTL(G_DL, 24, 8) | _SHIFTL(((u32)(dl) & 0xFFFFFFFF) >> 8, 0, 24))

#define gsSPEndDisplayList() \
    (_SHIFTL(G_ENDDL, 24, 8))

#define gsSPSetGeometryMode(word) \
    (_SHIFTL(G_SETGEOMETRYMODE, 24, 8) | _SHIFTL((word), 0, 24))

#define gsSPClearGeometryMode(word) \
    (_SHIFTL(G_CLEARGEOMETRYMODE, 24, 8) | _SHIFTL((word), 0, 24))

#define gsSPTexture(s, t, level, tile, on) \
    (_SHIFTL(G_TEXTURE, 24, 8) | _SHIFTL((level), 11, 3) | \
     _SHIFTL((tile), 8, 3) | _SHIFTL((on), 0, 8) | \
     _SHIFTL((s), 16, 16) | _SHIFTL((t), 0, 16))

#define gsDPPipeSync() \
    (_SHIFTL(G_RDPPIPESYNC, 24, 8))

#define gsDPFullSync() \
    (_SHIFTL(G_RDPFULLSYNC, 24, 8))

#define gsDPLoadTextureBlock(timg, fmt, siz, width, height, pal, cms, cmt, masks, maskt, shifts, shiftt) \
    gsDPSetTextureImage(fmt, siz, 1, timg), \
    gsDPSetTile(fmt, siz, 0, 0, G_TX_LOADTILE, 0, cmt, maskt, shiftt, cms, masks, shifts), \
    gsDPLoadSync(), \
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((width) * (height) + siz##_INCR(siz)) >> siz##_SHIFT(siz) - 1, 0), \
    gsDPPipeSync(), \
    gsDPSetTile(fmt, siz, (((width) * siz##_LINE_BYTES(siz)) + 7) >> 3, 0, G_TX_RENDERTILE, pal, cmt, maskt, shiftt, cms, masks, shifts), \
    gsDPSetTileSize(G_TX_RENDERTILE, 0, 0, ((width) - 1) << G_TEXTURE_IMAGE_FRAC, ((height) - 1) << G_TEXTURE_IMAGE_FRAC)

// Dynamic command construction (for gDisplayListHead++)
#define gSPMatrix(pkt, mtx, param) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsSPMatrix(mtx, param); \
        (pkt) = (void *)_g; \
    }

#define gSPPopMatrix(pkt, n) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsSPPopMatrix(n); \
        (pkt) = (void *)_g; \
    }

#define gSPVertex(pkt, v, n, v0) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsSPVertex(v, n, v0); \
        (pkt) = (void *)_g; \
    }

#define gSP1Triangle(pkt, v0, v1, v2, flag) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsSP1Triangle(v0, v1, v2, flag); \
        (pkt) = (void *)_g; \
    }

#define gSPDisplayList(pkt, dl) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsSPDisplayList(dl); \
        (pkt) = (void *)_g; \
    }

#define gSPEndDisplayList(pkt) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsSPEndDisplayList(); \
        (pkt) = (void *)_g; \
    }

#define gSPSetGeometryMode(pkt, word) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsSPSetGeometryMode(word); \
        (pkt) = (void *)_g; \
    }

#define gSPClearGeometryMode(pkt, word) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsSPClearGeometryMode(word); \
        (pkt) = (void *)_g; \
    }

#define gSPTexture(pkt, s, t, level, tile, on) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsSPTexture(s, t, level, tile, on); \
        (pkt) = (void *)_g; \
    }

#define gDPPipeSync(pkt) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsDPPipeSync(); \
        (pkt) = (void *)_g; \
    }

#define gDPFullSync(pkt) \
    { \
        Gfx *_g = (Gfx *)(pkt); \
        *_g++ = gsDPFullSync(); \
        (pkt) = (void *)_g; \
    }

#define gDPSetCombineMode(pkt, mode1, mode2)
#define gDPSetRenderMode(pkt, mode1, mode2)
#define gDPSetCycleType(pkt, type)
#define gDPSetTextureFilter(pkt, filter)
#define gDPSetTexturePersp(pkt, persp)
#define gDPSetTextureLOD(pkt, lod)
#define gDPSetTextureLUT(pkt, lut)
#define gDPSetTextureDetail(pkt, detail)
#define gDPSetTextureConvert(pkt, convert)
#define gDPSetCombineKey(pkt, key)
#define gDPSetAlphaCompare(pkt, cmp)
#define gDPSetColorDither(pkt, dither)
#define gDPSetAlphaDither(pkt, dither)
#define gDPSetPrimDepth(pkt, z, dz)
#define gDPSetScissor(pkt, mode, ulx, uly, lrx, lry)
#define gDPSetScissorFrac(pkt, mode, ulx, uly, lrx, lry)
#define gDPSetFillColor(pkt, color)
#define gDPSetFogColor(pkt, r, g, b, a)
#define gDPSetBlendColor(pkt, r, g, b, a)
#define gDPSetEnvColor(pkt, r, g, b, a)
#define gDPSetPrimColor(pkt, lodmin, lodfrac, r, g, b, a)
#define gDPSetDepthImage(pkt, img)
#define gDPSetColorImage(pkt, fmt, siz, width, img)
#define gDPSetTextureImage(pkt, fmt, siz, width, img)
#define gDPSetTile(pkt, fmt, siz, line, tmem, tile, palette, cmt, maskt, shiftt, cms, masks, shifts)
#define gDPSetTileSize(pkt, tile, uls, ult, lrs, lrt)
#define gDPLoadTile(pkt, tile, uls, ult, lrs, lrt)
#define gDPLoadBlock(pkt, tile, uls, ult, lrs, dxt)
#define gDPLoadTLUTCmd(pkt, tile, count)
#define gDPLoadSync(pkt)
#define gDPTileSync(pkt)
#define gDPPipelineMode(pkt, mode)
#define gDPSetAlphaCompare(pkt, mode)
#define gDPSetAlphaDither(pkt, mode)
#define gDPSetColorDither(pkt, mode)
#define gDPSetConvert(pkt, k0, k1, k2, k3, k4, k5)
#define gSPClipRatio(pkt, ratio)
#define gSPLoadGeometryMode(pkt, word)
#define gSPInsertMatrix(pkt, where, num)
#define gSPModifyVertex(pkt, vtx, where, val)
#define gSPNumLights(pkt, n)
#define gSPLight(pkt, l, n)
#define gSPLightColor(pkt, n, col)
#define gSPFogFactor(pkt, fm, fo)
#define gSPFogPosition(pkt, min, max)
#define gSPSetLights0(pkt)
#define gSPSetLights1(pkt, name)
#define gSPSetLights2(pkt, name)
#define gSPSetLights3(pkt, name)
#define gSPSetLights4(pkt, name)
#define gSPSetLights5(pkt, name)
#define gSPSetLights6(pkt, name)
#define gSPSetLights7(pkt, name)
#define gSPCopyLight(pkt, dest, src)
#define gSPAmbient(pkt, r, g, b)
#define gSPCamera(pkt, m)
#define gSPPerspective(pkt, fovy, aspect, near, far, scale)
#define gSPFrustum(pkt, l, r, b, t, n, f, s)
#define gSPOrtho(pkt, l, r, b, t, n, f, s)

// Sprite macros
#define gSPSprite2DBase(pkt, sprite)
#define gSPSprite2D(pkt, sprite, mtx, parm)
#define gSPSprite2DScale(pkt, sprite, mtx, parm)
#define gSPSprite2DDraw(pkt, sprite)
#define gSPSprite2DDrawScaled(pkt, sprite, mtx, parm)

// Lights
#define NUMPACKEDLIGHT 7

// Light structure
typedef struct {
    u8 col[3];
    char pad1;
    u8 colc[3];
    char pad2;
    s8 dir[3];
    char pad3;
} Light_t;

typedef struct {
    u8 col[3];
    char pad1;
    u8 colc[3];
    char pad2;
    s8 pos[3];
    char pad3;
} PosLight_t;

typedef union {
    Light_t l;
    PosLight_t p;
    long long int force_structure_alignment[2];
} Light;

typedef struct {
    Light l[2];
} Lights2;

#define Lights0 Lights2
#define Lights1 Lights2

// Vertex structure
typedef struct {
    s16 ob[3];
    s16 pad;
    s16 tc[2];
    u8  cn[4];
} Vtx_t;

typedef union {
    Vtx_t v;
    long long int force_structure_alignment[1];
} Vtx;

// Viewport
typedef struct {
    s16 vscale[4];
    s16 vtrans[4];
} Vp_t;

typedef union {
    Vp_t vp;
    long long int force_structure_alignment[1];
} Vp;

// Matrix
typedef struct {
    s16 intpart[4][4];
    u16 fracpart[4][4];
} Mtx_t;

typedef union {
    Mtx_t m;
    long long int force_structure_alignment[1];
    float f[4][4];
} Mtx;

// Color macros
#define GPACK_RGBA5551(r, g, b, a) \
    ((((r) & 0xF8) << 8) | (((g) & 0xF8) << 3) | (((b) & 0xF8) >> 2) | ((a) & 1))

#define GPACK_ZDZ(z, dz) \
    ((u32)((z) & 0x3FF) << 6 | ((dz) & 0x3F))

// Constants
#define G_MAXFBZ 0x3FFF
#define G_TEXTURE_IMAGE_FRAC 2
#define G_TX_FRAC 6

#define FRUSTRATIO_1 1
#define FRUSTRATIO_2 2
#define FRUSTRATIO_3 3
#define FRUSTRATIO_4 4
#define FRUSTRATIO_5 5
#define FRUSTRATIO_6 6

#define G_OFF 0
#define G_ON 1
#define G_AC_NONE 0
#define G_AC_THRESHOLD 1
#define G_AC_DITHER 3
#define G_ZS_PIXEL 0
#define G_ZS_PRIM 1
#define G_RM_PASS 0
#define G_RM_AA_ZB_OPA_INTER 0
#define G_RM_AA_ZB_OPA_DECAL 0
#define G_RM_RA_ZB_OPA_SURF 0
#define G_RM_AA_ZB_XLU_INTER 0
#define G_RM_AA_ZB_XLU_DECAL 0
#define G_RM_AA_ZB_XLU_SURF 0
#define G_RM_AA_ZB_XLU_SURF2 0
#define G_RM_RA_ZB_OPA_DECAL 0
#define G_RM_AA_XLU_SURF 0
#define G_RM_AA_XLU_DECAL 0
#define G_RM_AA_DECAL 0
#define G_RM_AA_ZB_OPA_TERR 0
#define G_RM_AA_ZB_TEX_TERR 0
#define G_RM_AA_ZB_SUB_TERR 0
#define G_RM_AA_ZB_PCL_SURF 0
#define G_RM_AA_ZB_TEX_EDGE 0
#define G_RM_AA_ZB_TEX_EDGE2 0
#define G_RM_AA_ZB_TEX_INTER 0
#define G_RM_AA_ZB_SUB_SURF 0
#define G_RM_AA_ZB_SUB_DECAL 0
#define G_RM_ZB_OPA_SURF2 3
#define G_RM_TEX_EDGE_DECAL 0
#define G_RM_TEX_EDGE_DECAL2 0
#define G_RM_AA_ZB_SUB_TERR 0
#define G_RM_AA_OPA_TERR 0
#define G_RM_AA_TEX_TERR 0
#define G_RM_AA_SUB_TERR 0
#define G_RM_AA_PCL_SURF 0
#define G_RM_AA_TEX_EDGE 0
#define G_RM_AA_TEX_INTER 0
#define G_RM_AA_SUB_SURF 0
#define G_RM_AA_SUB_DECAL 0
#define G_RM_AA_SUB_TERR 0
#define G_RM_NOOP 0
#define G_RM_VISCVG 0
#define G_RM_OPA_CI 0
#define G_RM_TEX_EDGE 0

#define G_TX_NOMIRROR 0
#define G_TX_WRAP 0
#define G_TX_MIRROR 1
#define G_TX_CLAMP 2
#define G_TX_NOMASK 0
#define G_TX_NOLOD 0
#define G_TX_LOADTILE 7
#define G_TX_RENDERTILE 0
#define G_TX_WRAP 0
#define G_TX_MIRROR 1
#define G_TX_CLAMP 2
#define G_IM_FMT_RGBA 0
#define G_IM_FMT_YUV 1
#define G_IM_FMT_CI 2
#define G_IM_FMT_IA 3
#define G_IM_FMT_I 4
#define G_IM_SIZ_4b 0
#define G_IM_SIZ_8b 1
#define G_IM_SIZ_16b 2
#define G_IM_SIZ_32b 3

#define G_CYC_1CYCLE 0
#define G_CYC_2CYCLE 1
#define G_CYC_COPY 2
#define G_CYC_FILL 3

#define G_PM_NPRIMITIVE 0
#define G_PM_1PRIMITIVE 1

#define G_TP_NONE 0
#define G_TP_PERSP 1

#define G_TD_CLAMP 0
#define G_TD_SHARPEN 1
#define G_TD_DETAIL 2

#define G_TL_TILE 0
#define G_TL_LOD 1

#define G_TF_POINT 0
#define G_TF_AVERAGE 3
#define G_TF_BILERP 2

#define G_TT_NONE 0
#define G_TT_RGBA16 2
#define G_TT_IA16 3

#define G_TC_CONV 0
#define G_TC_FILTCONV 5
#define G_TC_FILT 6

#define G_CK_NONE 0
#define G_CK_REF 1

#define G_CD_MAGICSQ 0
#define G_CD_BAYER 1
#define G_CD_NOISE 2
#define G_CD_DISABLE 3

#define G_AD_PATTERN 0
#define G_AD_NOTPATTERN 1
#define G_AD_NOISE 2
#define G_AD_DISABLE 3

#define G_SC_NON_INTERLACE 0
#define G_SC_ODD_INTERLACE 2
#define G_SC_EVEN_INTERLACE 3

// Screen dimensions
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// GBI inline functions - stubbed out for now
// static inline void gSPLight(Gfx *pkt, Light *l, s32 n) { }

#endif // _GBI_H_

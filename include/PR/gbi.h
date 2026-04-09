#ifndef _GBI_H_
#define _GBI_H_

// N64 GBI (Graphics Binary Interface) header for 3DS
// Minimal stub - full implementation in gbi_3ds.h

#include "ultratypes.h"

// Basic GBI types
typedef u64 Gfx;

// Display list command opcodes (minimal set)
#define G_NOOP          0x00
#define G_VTX           0x01
#define G_DL            0x06
#define G_TRI1          0xBF
#define G_ENDDL         0xDF
#define G_MTX           0x01
#define G_TEXTURE       0xD7
#define G_GEOMETRYMODE  0xD9
#define G_POPMTX        0xD8
#define G_SETOTHERMODE_H 0xE2
#define G_SETOTHERMODE_L 0xE3
#define G_RDPHALF_1     0xE1
#define G_RDPHALF_2     0xF1
#define G_TEXTURE_IMAGE_FRAC 2

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

// Light structure
typedef struct {
    u8 col[3];
    char pad1;
    u8 colc[3];
    char pad2;
    s8 dir[3];
    char pad3;
} Light_t;

typedef union {
    Light_t l;
    long long int force_structure_alignment[2];
} Light;

typedef struct {
    Light l[2];
} Lights2;

#define Lights0 Lights2
#define Lights1 Lights2

// Macros
#define _SHIFTL(v, s, w) ((u32)((u32)(v) & ((0x01 << (w)) - 1)) << (s))
#define _SHIFTR(v, s, w) ((u32)((u32)(v) >> (s)) & ((0x01 << (w)) - 1))

// Geometry mode macros
#define gSPSetGeometryMode(pkt, word)  \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = (_SHIFTL(G_GEOMETRYMODE, 24, 8) | _SHIFTL((word), 0, 24)); (pkt) = (void *)_g; }

#define gSPClearGeometryMode(pkt, word) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = (_SHIFTL(G_GEOMETRYMODE, 24, 8) | _SHIFTL((~(word) & 0x00FFFFFF), 0, 24)); (pkt) = (void *)_g; }

#define gSPGeometryMode(pkt, c, s) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = (_SHIFTL(G_GEOMETRYMODE, 24, 8) | _SHIFTL((~(c) & 0x00FFFFFF), 0, 24) | _SHIFTL((s), 0, 24)); (pkt) = (void *)_g; }

// Vertex macros
#define gSPVertex(pkt, v, n, v0) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = (_SHIFTL(G_VTX, 24, 8) | _SHIFTL((n), 12, 8) | _SHIFTL((v0) + (n), 1, 7) | _SHIFTL(((u32)(v) >> 8) & 0xFFFF, 0, 16)); (pkt) = (void *)_g; }

// Triangle macros
#ifdef _3DS
// 3DS-specific GBI macros in src/port/3ds/gbi.h provide implementations
#else
#ifndef gSP1Triangle
#define gSP1Triangle(pkt, v0, v1, v2, flag) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = (_SHIFTL(G_TRI1, 24, 8) | _SHIFTL((flag), 23, 1) | _SHIFTL((v0) * 2, 16, 8) | _SHIFTL((v1) * 2, 8, 8) | _SHIFTL((v2) * 2, 0, 8)); (pkt) = (void *)_g; }
#endif

// Display list macros
#ifndef gSPDisplayList
#define gSPDisplayList(pkt, dl) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = (_SHIFTL(G_DL, 24, 8) | _SHIFTL(((u32)(dl) & 0xFFFFFFFF) >> 8, 0, 24)); (pkt) = (void *)_g; }
#endif

#ifndef gSPEndDisplayList
#define gSPEndDisplayList(pkt) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = _SHIFTL(G_ENDDL, 24, 8); (pkt) = (void *)_g; }
#endif
#endif

#define gSPBranchList(pkt, dl) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = (_SHIFTL(G_DL, 24, 8) | _SHIFTL(1, 16, 8) | _SHIFTL(((u32)(dl) & 0xFFFFFFFF) >> 8, 0, 24)); (pkt) = (void *)_g; }

// Matrix macros
#define gSPMatrix(pkt, m, p) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = (_SHIFTL(G_MTX, 24, 8) | _SHIFTL((p), 16, 8) | _SHIFTL(((u32)(m) >> 8) & 0xFFFF, 0, 16)); (pkt) = (void *)_g; }

#define gSPPopMatrix(pkt, n) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = (_SHIFTL(G_POPMTX, 24, 8) | _SHIFTL((n), 16, 8)); (pkt) = (void *)_g; }

// Texture macros
#define gSPTexture(pkt, s, t, level, tile, on) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = (_SHIFTL(G_TEXTURE, 24, 8) | _SHIFTL((level), 11, 3) | _SHIFTL((tile), 8, 3) | _SHIFTL((on), 0, 8) | _SHIFTL((s), 16, 16) | _SHIFTL((t), 0, 16)); (pkt) = (void *)_g; }

// Sync macros
#ifdef _3DS
// 3DS-specific GBI macros in src/port/3ds/gbi.h provide implementations
#else
#ifndef gDPPipeSync
#define gDPPipeSync(pkt) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = _SHIFTL(0xE7, 24, 8); (pkt) = (void *)_g; }
#endif
#endif

#define gDPLoadSync(pkt) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = _SHIFTL(0xE6, 24, 8); (pkt) = (void *)_g; }

#define gDPTileSync(pkt) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = _SHIFTL(0xE8, 24, 8); (pkt) = (void *)_g; }

#define gDPFullSync(pkt) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = _SHIFTL(0xE9, 24, 8); (pkt) = (void *)_g; }

// Additional common macros
#define gSPNoOp(pkt) \
    { Gfx *_g = (Gfx *)(pkt); *_g++ = _SHIFTL(G_NOOP, 24, 8); (pkt) = (void *)_g; }

#define gsSPNoOp() \
    _SHIFTL(G_NOOP, 24, 8)

// Texture formats
#define G_IM_FMT_RGBA   0
#define G_IM_FMT_YUV    1
#define G_IM_FMT_CI     2
#define G_IM_FMT_IA     3
#define G_IM_FMT_I      4

#define G_IM_SIZ_4b     0
#define G_IM_SIZ_8b     1
#define G_IM_SIZ_16b    2
#define G_IM_SIZ_32b    3

// Constants
#define G_MAXFBZ 0x3FFF
#define G_TX_FRAC 6

#define FRUSTRATIO_1 1
#define FRUSTRATIO_2 2

#define G_OFF 0
#define G_ON 1

#define G_AC_NONE 0
#define G_AC_THRESHOLD 1
#define G_ZS_PIXEL 0
#define G_ZS_PRIM 1

#define G_RM_PASS 0

// Texture parameters
#define G_TX_NOMIRROR   0
#define G_TX_WRAP       0
#define G_TX_MIRROR     1
#define G_TX_CLAMP      2
#define G_TX_NOMASK     0
#define G_TX_NOLOD      0
#define G_TX_LOADTILE   7
#define G_TX_RENDERTILE 0

// Render mode
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

// Color macros
#define GPACK_RGBA5551(r, g, b, a) \
    ((((r) & 0xF8) << 8) | (((g) & 0xF8) << 3) | (((b) & 0xF8) >> 2) | ((a) & 1))

#define GPACK_ZDZ(z, dz) \
    ((u32)((z) & 0x3FF) << 6 | ((dz) & 0x3F))

#endif // _GBI_H_

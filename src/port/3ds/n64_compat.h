#ifndef _N64_3DS_COMPAT_H
#define _N64_3DS_COMPAT_H

// N64 SDK compatibility layer for 3DS
// Maps N64 types and functions to 3DS equivalents

#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include "PR/gbi.h"

// Include ultratypes.h first for OSMesgQueue and OSMesg definitions
#include "PR/ultratypes.h"

// Forward declarations for N64 types
struct SPTask;
struct OSTask;
struct OSContPadEx;
struct OSContStatus;
struct OSViMode;

// Include N64 SDK headers for type definitions
#include "n64/PR/os_cont.h"
#include "n64/PR/sptask.h"

// ============================================================================
// N64 Type Definitions
// ============================================================================

typedef u8  u8;
typedef s8  s8;
typedef u16 u16;
typedef s16 s16;
typedef u32 u32;
typedef s32 s32;
typedef u64 u64;
typedef s64 s64;
// f32 and f64 defined in ultratypes.h as float and double
// typedef f32 f32;
// typedef f64 f64;

// N64 Boolean - defined in types.h
// typedef s32 Bool32;
// #define FALSE 0
// #define TRUE  1

// N64 OS Types - defined in PR/ultratypes.h, just declare externs here
typedef u32 OSTime;
// typedef u32 OSMesg; // Defined in ultratypes.h as void*
// typedef s32 OSIntMask; // Defined in ultratypes.h as u32

// Task state constants - defined in types.h
// #define SPTASK_STATE_NOT_STARTED  0
// #define SPTASK_STATE_RUNNING      1
// #define SPTASK_STATE_INTERRUPTED  2
// #define SPTASK_STATE_FINISHED     3
// #define SPTASK_STATE_FINISHED_DP  4

// SPTask structure defined in types.h

#define M_AUDTASK  2
#define M_GFXTASK  1

#define MAXCONTROLLERS  4

// Thread types - defined in ultratypes.h
// typedef struct { ... } OSThread; 

// Memory
typedef void *OSIoMesg;

// ============================================================================
// N64 Button Constants
// ============================================================================
// Button constants defined in os_cont.h

#define END_DEMO       0xFFFF

// ============================================================================
// N64 Constants
// ============================================================================

#define SCREEN_WIDTH      320
#define SCREEN_HEIGHT     240
// #define GFX_POOL_SIZE     (1024 * 512)  // Defined in config_graphics.h
#define SEGMENT_MAIN      0
#define SEGMENT_RENDER    1
#define SEGMENT_MARIO_ANIMS 2
#define SEGMENT_DEMO_INPUTS 3
#define SEGMENT_LEVEL_ENTRY 4
#define SEGMENT_SEGMENT2    5

// ============================================================================
// Memory
// ============================================================================

#define RAM_START   0x80000000
// #define PHYSICAL_TO_VIRTUAL(addr) ((uintptr_t)(addr) | 0x80000000) // Defined in macros.h
// #define VIRTUAL_TO_PHYSICAL(addr) ((uintptr_t)(addr) & ~0x80000000) // Defined in macros.h)

// segmented_to_virtual defined in memory.h
// static inline void *segmented_to_virtual(void *addr) {
//     return addr;
// }

// Memory allocation - defined in memory.h
// typedef enum {
//     MEMORY_POOL_LEFT,
//     MEMORY_POOL_RIGHT
// } MemoryPoolSide;

// void *main_pool_alloc(u32 size, s32 side);
// void main_pool_free(void *ptr);
void *main_pool_realloc(void *ptr, u32 size);

// ============================================================================
// Threading (3DS-compatible)
// ============================================================================

void osCreateThread(OSThread *t, s32 id, void (*func)(void *), void *arg, void *sp, s32 pri);
void osStartThread(OSThread *t);
void osStopThread(OSThread *t);
void osYieldThread(void);

// ============================================================================
// Message Queues
// ============================================================================

void osCreateMesgQueue(OSMesgQueue *mq, OSMesg *msg, s32 count);
s32 osSendMesg(OSMesgQueue *mq, OSMesg msg, s32 flags);
s32 osRecvMesg(OSMesgQueue *mq, OSMesg *msg, s32 flags);

#define OS_MESG_BLOCK      1
#define OS_MESG_NOBLOCK    0

// ============================================================================
// Timing
// ============================================================================

void osSetTimer(void *timer, u16 countdown, u64 interval, OSMesgQueue *mq, OSMesg msg);
// OSTime osGetTime(void); // Provided by 3DS SDK
#define OS_USEC_TO_CYCLES(usec) ((usec) * 93)  // ~93.75 MHz N64

// ============================================================================
// Video Interface
// ============================================================================

void osViSetMode(OSViMode *mode);
void osViSetEvent(OSMesgQueue *mq, OSMesg msg, u32 retraceCount);
void osViSwapBuffer(void *vaddr);
void osViSetSpecialFeatures(u32 func);

#define OS_VI_DITHER_FILTER_ON  0x0001
#define OS_VI_GAMMA_OFF         0x0008
#define OS_VI_GAMMA_DITHER_ON   0x0010

// ============================================================================
// Controllers
// ============================================================================

// s32 osContInit(OSMesgQueue *mq, u8 *bitpattern, OSContStatus *status); // Not used on 3DS
void osContStartReadDataEx(OSMesgQueue *mq);
// void osContGetReadDataEx(OSContPadEx *pad); // Not used on 3DS
void osContSetCh(u8 enable);

// ============================================================================
// Events
// ============================================================================
// Event macros defined in os_message.h

typedef u32 OSEvent;

void osSetEventMesg(OSEvent e, OSMesgQueue *mq, OSMesg msg);

// ============================================================================
// EEPROM / SRAM
// ============================================================================

s32 osEepromProbe(OSMesgQueue *mq);
s32 osEepromProbeVC(OSMesgQueue *mq);
s32 osEepromRead(OSMesgQueue *mq, u8 address, u8 *buffer);
s32 osEepromWrite(OSMesgQueue *mq, u8 address, u8 *buffer);

// SRAM
s32 nuPiInitSram(void);
s32 nuPiReadSram(u32 romaddr, void *ramaddr, u32 len);
s32 nuPiWriteSram(u32 romaddr, void *ramaddr, u32 len);

// ============================================================================
// Display List
// ============================================================================
// Gfx is defined in include/PR/gbi.h

// Graphics pool - defined in game_init.h
// #define GFX_POOL_SIZE 25000 // Defined in config_graphics.h

// struct GfxPool {
//     // Gfx buffer[GFX_POOL_SIZE / sizeof(Gfx)]; // Not used on 3DS
//     // SPTask spTask; // Not used on 3DS
// };

// ============================================================================
// Cache
// ============================================================================
// Cache operations defined in ultratypes.h

// ============================================================================
// Math
// ============================================================================
// Mtx defined in gbi.h
// typedef f32 Mtx[4][4];
typedef float Mat4[4][4];

// Math functions in standard library
// f32 sqrtf(f32 x);
// f32 sinf(f32 x);
// f32 cosf(f32 x);

// ============================================================================
// Debug
// ============================================================================

void osSyncPrintf(const char *fmt, ...);

// ============================================================================
// 3DS Integration Externals
// ============================================================================

// From platform layer
extern void platform_3ds_init(void);
extern void platform_3ds_exit(void);
extern void platform_3ds_vblank_wait(void);
extern u32 platform_3ds_get_buttons(void);
extern void platform_3ds_get_stick(s8 *x, s8 *y);

// Audio - defined in audio_3ds.h
// extern void audio_3ds_init(void);
// extern void audio_3ds_submit_samples(s16 *data, u32 samples);

// Save - defined in save_3ds.h
// extern void save_3ds_init(void);
// extern s32 save_3ds_read(void *data, u32 len);
// extern s32 save_3ds_write(void *data, u32 len);

// DMA - defined in memory.h
// extern void dma_read(void *ramAddr, u32 romAddr, u32 len);

// Memory
extern void my_free(void *ptr);

// types.h not included here to avoid circular dependency
// Include it directly in .c files that need it

#endif // _N64_3DS_COMPAT_H_

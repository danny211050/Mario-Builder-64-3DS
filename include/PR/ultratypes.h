#ifndef _ULTRATYPES_H_
#define _ULTRATYPES_H_

#include <stdint.h>

// Basic types using stdint to match platform exactly
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float f32;
typedef double f64;

// Additional N64 types
typedef u32 OSTime;
typedef u32 OSIntMask;
typedef void* OSMesg;

// Message queue
typedef struct {
    s32 validCount;
    s32 first;
    s32 msgCount;
    OSMesg *msg;
    void *threadQueue;
} OSMesgQueue;

// Controller types - defined in n64/PR/os_cont.h, just declare here
struct OSContStatus;
struct OSContPadEx;

// Task structure - defined in types.h, just declare here
struct OSTask;
struct SPTask;

// Thread
typedef struct OSThread_s {
    struct OSThread_s *next;
    u32 priority;
    void *queue;
    void *context;
} OSThread;

// Video
typedef struct {
    u32 mode;
    u32 width;
    u32 height;
} OSViMode;

// Utility macros
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

// Alignment
#define ALIGNED8 __attribute__((aligned(8)))
#define ALIGNED16 __attribute__((aligned(16)))
#define ALIGNED32 __attribute__((aligned(32)))
#define ALIGNED64 __attribute__((aligned(64)))

// N64 cache operations (no-ops on 3DS)
#define osInvalDCache(ptr, size)       ((void)0)
#define osWritebackDCache(ptr, size)   ((void)0)
#define osWritebackDCacheAll()         ((void)0)
#define osInvalICache(ptr, size)       ((void)0)

#endif // _ULTRATYPES_H_

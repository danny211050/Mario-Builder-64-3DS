#ifndef _ULTRA64_H_
#define _ULTRA64_H_

// N64 SDK compatibility header for 3DS
// Maps N64 SDK headers to 3DS equivalents

#include "n64_compat.h"

// N64 types
#ifndef u8_typedef
#define u8_typedef
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed long s32;
typedef unsigned long long u64;
typedef signed long long s64;
typedef float f32;
typedef double f64;
#endif

// Common N64 constants
#define NULL 0
#define FALSE 0
#define TRUE 1

// Memory alignment
#define ALIGN16(val) (((val) + 0xF) & ~0xF)
#define ALIGN32(val) (((val) + 0x1F) & ~0x1F)
#define ALIGN64(val) (((val) + 0x3F) & ~0x3F)

// N64 segments
#define K0BASE 0x80000000
#define K1BASE 0xA0000000
#define K2BASE 0xC0000000

// OS Functions
#define osPiStartDma(...)
#define osPiGetStatus() 0

// PI (Peripheral Interface)
#define osPiRawStartDma(...) 0
#define osPiReadIo(...) 0
#define osPiWriteIo(...) 0

// SI (Serial Interface)
#define osSiGetStatus() 0
#define osSiRawReadIo(...) 0
#define osSiRawWriteIo(...) 0

// AI (Audio Interface)
#define osAiGetStatus() 0
#define osAiSetFrequency(freq) (freq)
#define osAiSetNextBuffer(buf, size)

// DP (Display Processor)
#define osDpGetStatus() 0
#define osDpSetStatus(val)
#define osDpGetCounters(c)

// SP (Signal Processor)
#define osSpGetStatus() 0
#define osSpSetStatus(val)
#define osSpTaskLoad(task)
#define osSpTaskStartGo(task)
#define osSpTaskYield()
#define osSpTaskYielded(task) 0

// VI (Video Interface)
#define osViGetCurrentLine() 0
#define osViGetCurrentField() 0
#define osViGetNextFramebuffer() NULL
#define osViGetStatus() 0

// Flash RAM
#define osFlashInit()
#define osFlashReadId(...)
#define osFlashReadBuffer(...)
#define osFlashWriteBuffer(...)
#define osFlashReadArray(...)
#define osFlashWriteArray(...)
#define osFlashEraseChip()
#define osFlashEraseBlock(...)
#define osFlashChange(...)

// Drive
#define osDriveRomInit()

// Exception
#define osSetErrorHandler(handler)
#define osGetMemSize() (8 * 1024 * 1024)  // 8MB RAM

// Thread
#define osDestroyThread(thread)
#define osGetThreadId(thread) 0
#define osGetThreadPri(thread) 0
#define osSetThreadPri(thread, pri)

// Message
#define osJamMesg(mq, msg, flags) osSendMesg(mq, msg, flags)

// Event
#define osGetCount() osGetTime()

// Macro
#define OS_MESG_PRI_NORMAL 0
#define OS_MESG_PRI_HIGH 1

// Cache
#define osPiGetCmdQueue() NULL
#define osSetTLBASID(...) 
#define osUnmapTLB(...)
#define osUnmapTLBAll()
#define osWritebackDCache(ptr, size)
#define osWritebackDCacheAll()
#define osInvalDCache(ptr, size)
#define osInvalICache(ptr, size)
#define osInvalTLBPage(page)

// Thread states
#define OS_STATE_STOPPED 1
#define OS_STATE_RUNNABLE 2
#define OS_STATE_RUNNING 4
#define OS_STATE_WAITING 8

// Message flags
#define OS_MESG_BLOCK 1
#define OS_MESG_NOBLOCK 0

// Thread flags
#define OS_FLAG_STATE_MASK 0x03

// Pi
#define OS_READ 0
#define OS_WRITE 1
#define OS_MESG_SKIP 0x80000000

// N64 Graphics Binary Interface (GBI)
#include "gbi.h"

// N64 Sound
#include "n64_sound.h"

// N64 Utilities
#include "n64_utils.h"

#endif // _ULTRA64_H_

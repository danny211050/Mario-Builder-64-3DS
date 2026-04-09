#include "n64_compat.h"
#include "types.h"
#include "save_3ds.h"
#include "romfs_3ds.h"
#include "gbi_3ds.h"
#include <citro3d.h>
#include <stdio.h>

// Define memory pool constants
#define MEMORY_POOL_LEFT 0
#define MEMORY_POOL_RIGHT 1

// ============================================================================
// N64 Compatibility Layer Implementation
// ============================================================================

static u64 g_ostime_base = 0;

// Threading stubs (3DS is single-threaded for game loop)
void osCreateThread(OSThread *t, s32 id, void (*func)(void *), void *arg, void *sp, s32 pri) {
    // Store thread info - actual threading handled by 3DS platform
    memset(t, 0, sizeof(OSThread));
}

void osStartThread(OSThread *t) {
    // Threads started by platform
}

void osStopThread(OSThread *t) {
    // Threads stopped by platform
}

void osYieldThread(void) {
    // Yield to 3DS scheduler
}

// Message queues (simplified for 3DS)
typedef struct {
    OSMesgQueue *queue;
    OSMesg msg;
    u32 valid;
} SimpleMesg;

static SimpleMesg g_mesg_slots[32];
static int g_mesg_count = 0;

void osCreateMesgQueue(OSMesgQueue *mq, OSMesg *msg, s32 count) {
    memset(mq, 0, sizeof(OSMesgQueue));
    mq->msg = msg;
    mq->msgCount = count;
}

s32 osSendMesg(OSMesgQueue *mq, OSMesg msg, s32 flags) {
    if (g_mesg_count < 32) {
        g_mesg_slots[g_mesg_count].queue = mq;
        g_mesg_slots[g_mesg_count].msg = msg;
        g_mesg_slots[g_mesg_count].valid = 1;
        g_mesg_count++;
    }
    return 0;
}

s32 osRecvMesg(OSMesgQueue *mq, OSMesg *msg, s32 flags) {
    // Check for messages in queue
    for (int i = 0; i < g_mesg_count; i++) {
        if (g_mesg_slots[i].valid && g_mesg_slots[i].queue == mq) {
            *msg = g_mesg_slots[i].msg;
            g_mesg_slots[i].valid = 0;
            return 0;
        }
    }
    
    // If blocking, wait for vblank
    if (flags == OS_MESG_BLOCK) {
        platform_3ds_vblank_wait();
    }
    
    return 0;
}

// Timing
void osSetTimer(void *timer, u16 countdown, u64 interval, OSMesgQueue *mq, OSMesg msg) {
    // Timer support via 3DS
}

OSTime n64GetTime(void) {
    // Return time in microseconds since start
    u64 current_time = svcGetSystemTick();
    return (OSTime)((current_time - g_ostime_base) * 1000 / SYSCLOCK_ARM11);
}

// Video Interface
void osViSetMode(OSViMode *mode) {
    // VI mode set by platform init
}

void osViSetEvent(OSMesgQueue *mq, OSMesg msg, u32 retraceCount) {
    // Vblank events handled by platform
}

void osViSwapBuffer(void *vaddr) {
    // Framebuffer swap handled by platform
    platform_3ds_vblank_wait();
}

void osViSetSpecialFeatures(u32 func) {
    // Special features not applicable on 3DS
}

// Controllers
s32 osContInit(OSMesgQueue *mq, u8 *bitpattern, OSContStatus *status) {
    *bitpattern = 0x01; // Controller 1 present
    if (status) {
        status[0].type = 0x0500; // N64 controller
        status[0].status = 0;
    }
    return 0;
}

void osContStartReadDataEx(OSMesgQueue *mq) {
    // Input read starts
}

// static OSContPadEx g_pad_data[MAXCONTROLLERS];

void osContGetReadDataEx(OSContPadEx *pad) {
    // Get input from 3DS platform
    u32 buttons = platform_3ds_get_buttons();
    s8 stick_x = 0, stick_y = 0;
    platform_3ds_get_stick(&stick_x, &stick_y);
    
    // Map 3DS buttons to N64
    pad[0].button = 0;
    if (buttons & KEY_A)      pad[0].button |= A_BUTTON;
    if (buttons & KEY_B)      pad[0].button |= B_BUTTON;
    if (buttons & KEY_X)      pad[0].button |= U_CBUTTONS;  // X = C-Up
    if (buttons & KEY_Y)      pad[0].button |= L_CBUTTONS;  // Y = C-Left
    if (buttons & KEY_L)      pad[0].button |= L_TRIG;
    if (buttons & KEY_R)      pad[0].button |= R_TRIG;
    if (buttons & KEY_ZL)     pad[0].button |= Z_TRIG;
    if (buttons & KEY_ZR)     pad[0].button |= Z_TRIG;
    if (buttons & KEY_START)  pad[0].button |= START_BUTTON;
    if (buttons & KEY_DUP)    pad[0].button |= U_JPAD;
    if (buttons & KEY_DDOWN)  pad[0].button |= D_JPAD;
    if (buttons & KEY_DLEFT)  pad[0].button |= L_JPAD;
    if (buttons & KEY_DRIGHT) pad[0].button |= R_JPAD;
    
    // C-Stick for C-buttons
    if (buttons & KEY_CSTICK_UP)    pad[0].button |= U_CBUTTONS;
    if (buttons & KEY_CSTICK_DOWN)  pad[0].button |= D_CBUTTONS;
    if (buttons & KEY_CSTICK_LEFT)  pad[0].button |= L_CBUTTONS;
    if (buttons & KEY_CSTICK_RIGHT) pad[0].button |= R_CBUTTONS;
    
    pad[0].stick_x = stick_x;
    pad[0].stick_y = stick_y;
    pad[0].errno = 0;
}

void osContSetCh(u8 enable) {
    // Set number of controllers
}

// Events
void osSetEventMesg(OSEvent e, OSMesgQueue *mq, OSMesg msg) {
    // Event registration
}

// EEPROM / SRAM stubs
s32 osEepromProbe(OSMesgQueue *mq) {
    // Return EEPROM present
    return 1;
}

s32 osEepromProbeVC(OSMesgQueue *mq) {
    return 1;
}

s32 osEepromRead(OSMesgQueue *mq, u8 address, u8 *buffer) {
    return save_3ds_read(buffer, 8);
}

s32 osEepromWrite(OSMesgQueue *mq, u8 address, u8 *buffer) {
    return save_3ds_write(buffer, 8);
}

s32 nuPiInitSram(void) {
    save_3ds_init();
    return 1;
}

s32 nuPiReadSram(u32 romaddr, void *ramaddr, u32 len) {
    return save_3ds_read(ramaddr, len);
}

s32 nuPiWriteSram(u32 romaddr, void *ramaddr, u32 len) {
    return save_3ds_write(ramaddr, len);
}

// Memory pool
static u8 g_main_pool[8 * 1024 * 1024]; // 8MB pool
static u32 g_pool_left = 0;
static u32 g_pool_right = sizeof(g_main_pool);

void *main_pool_alloc(u32 size, s32 side) {
    if (side == MEMORY_POOL_LEFT) {
        void *ptr = &g_main_pool[g_pool_left];
        g_pool_left += size;
        return ptr;
    } else {
        g_pool_right -= size;
        return &g_main_pool[g_pool_right];
    }
}

void main_pool_free(void *ptr) {
    // Simplified - no individual frees
}

void *main_pool_realloc(void *ptr, u32 size) {
    return main_pool_alloc(size, MEMORY_POOL_LEFT);
}

// Debug
void osSyncPrintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

// ============================================================================
// Segment Management
// ============================================================================

static void *g_segment_bases[16];

void set_segment_base_addr(s32 segment, void *addr) {
    if (segment >= 0 && segment < 16) {
        g_segment_bases[segment] = addr;
    }
}

void *get_segment_base_addr(s32 segment) {
    if (segment >= 0 && segment < 16) {
        return g_segment_bases[segment];
    }
    return NULL;
}

void move_segment_table_to_dmem(void) {
    // Segment table already in memory
}

// ============================================================================
// DMA
// ============================================================================

void dma_read(void *ramAddr, u32 romAddr, u32 len) {
    // Read from ROM to RAM via ROMFS
    (void)ramAddr;
    (void)romAddr;
    (void)len;
}

void load_segment(s32 segment, void *romStart, void *romEnd, s32 side, void *outSrc, void *outDst) {
    u32 size = (u32)romEnd - (u32)romStart;
    void *ramAddr = main_pool_alloc(size, side);
    dma_read(ramAddr, (u32)romStart, size);
    set_segment_base_addr(segment, ramAddr);
}

void load_segment_decompress(s32 segment, void *romStart, void *romEnd) {
    // Load compressed segment
    load_segment(segment, romStart, romEnd, MEMORY_POOL_LEFT, NULL, NULL);
}

// ============================================================================
// Display List Task
// ============================================================================

// extern void exec_display_list(struct SPTask *task);
extern void gbi_3ds_run_dl(N64Gfx *dl);

// void exec_display_list(SPTask *task) {
//     (void)task;
// }

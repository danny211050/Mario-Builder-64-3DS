#ifndef _3DS_INPUT_H
#define _3DS_INPUT_H

#include <3ds.h>
#include "platform_3ds.h"

// N64 button mappings to 3DS
// N64 controller: A, B, Z, Start, D-Pad, Stick, L, R, C-buttons
// 3DS controls: A, B, X, Y, L, R, Circle Pad, D-Pad, Touch, C-Stick (n3DS)

// Default button mapping
#define N64_BTN_A       0x8000
#define N64_BTN_B       0x4000
#define N64_BTN_Z       0x2000
#define N64_BTN_START   0x1000
#define N64_BTN_DUP     0x0800
#define N64_BTN_DDOWN   0x0400
#define N64_BTN_DLEFT   0x0200
#define N64_BTN_DRIGHT  0x0100
#define N64_BTN_L       0x0020
#define N64_BTN_R       0x0010
#define N64_BTN_CUP     0x0008
#define N64_BTN_CDOWN   0x0004
#define N64_BTN_CLEFT   0x0002
#define N64_BTN_CRIGHT  0x0001

// 3DS to N64 button mapping
typedef struct {
    u32 n64_button;      // N64 button mask
    u32 btn3ds_standard; // 3DS button (o3DS/n3DS)
    u32 btn3ds_alt;      // Alternative 3DS button
} ButtonMapping;

// Control stick / analog input
typedef struct {
    s8 x;  // -128 to 127
    s8 y;  // -128 to 127
} AnalogStick;

// Input state
typedef struct {
    u32 buttons_pressed;    // Buttons pressed this frame
    u32 buttons_held;       // Buttons currently held
    u32 buttons_released;   // Buttons released this frame
    AnalogStick stick;      // Main control stick (Circle Pad)
    AnalogStick cstick;     // C-stick (n3DS only, mapped to C-buttons)
    touchPosition touch;    // Touch screen position
    u8 touch_pressed;       // Touch screen pressed this frame
    u8 touch_held;          // Touch screen held
    u8 touch_released;      // Touch screen released this frame
} InputState;

// Initialize input system
void input_3ds_init(void);

// Update input (call once per frame)
void input_3ds_update(void);

// Get current input state
const InputState* input_3ds_get_state(void);

// Get N64 button state from 3DS input
u32 input_3ds_get_n64_buttons(void);

// Get N64 analog stick values
void input_3ds_get_n64_stick(s8 *x, s8 *y);

// Button remapping configuration
void input_3ds_load_mapping(const char *config_file);
void input_3ds_save_mapping(const char *config_file);
void input_3ds_set_mapping(u32 n64_button, u32 btn3ds);

// Touch screen menu/input handling
u8 input_3ds_touch_in_rect(u16 x, u16 y, u16 w, u16 h);
u8 input_3ds_touch_pressed_in_rect(u16 x, u16 y, u16 w, u16 h);

#endif // _3DS_INPUT_H

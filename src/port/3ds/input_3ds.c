#include "input_3ds.h"
#include <string.h>
#include <stdio.h>

// Default control mapping
static ButtonMapping g_default_mapping[] = {
    // Face buttons
    {N64_BTN_A,       KEY_A,        KEY_X},         // A -> 3DS A or X
    {N64_BTN_B,       KEY_B,        KEY_Y},         // B -> 3DS B or Y
    {N64_BTN_Z,       KEY_ZL,       KEY_ZR},        // Z -> ZL/ZR (n3DS) or L/R
    {N64_BTN_START,   KEY_START,    KEY_SELECT},    // Start -> Start or Select
    
    // D-Pad
    {N64_BTN_DUP,     KEY_DUP,      KEY_CPAD_UP},   // D-Up -> D-Pad Up or Circle Pad Up
    {N64_BTN_DDOWN,   KEY_DDOWN,    KEY_CPAD_DOWN}, // D-Down -> D-Pad Down or Circle Pad Down
    {N64_BTN_DLEFT,   KEY_DLEFT,    KEY_CPAD_LEFT}, // D-Left -> D-Pad Left or Circle Pad Left
    {N64_BTN_DRIGHT,  KEY_DRIGHT,   KEY_CPAD_RIGHT},// D-Right -> D-Pad Right or Circle Pad Right
    
    // Shoulder buttons
    {N64_BTN_L,       KEY_L,        KEY_ZL},        // L -> L or ZL
    {N64_BTN_R,       KEY_R,        KEY_ZR},        // R -> R or ZR
    
    // C-Buttons (for camera or extra controls)
    {N64_BTN_CUP,     KEY_CSTICK_UP,    KEY_X},     // C-Up -> C-Stick Up or X
    {N64_BTN_CDOWN,   KEY_CSTICK_DOWN,  KEY_B},     // C-Down -> C-Stick Down or B
    {N64_BTN_CLEFT,   KEY_CSTICK_LEFT,  KEY_Y},     // C-Left -> C-Stick Left or Y
    {N64_BTN_CRIGHT,  KEY_CSTICK_RIGHT, KEY_A},     // C-Right -> C-Stick Right or A
};

// Current mapping (can be customized)
static ButtonMapping g_current_mapping[16];
static int g_mapping_count = 0;

// Input state
static InputState g_input_state;
static u32 g_prev_buttons = 0;
static touchPosition g_prev_touch;

// Touch screen calibration
#define TOUCH_SCREEN_WIDTH 320
#define TOUCH_SCREEN_HEIGHT 240

void input_3ds_init(void) {
    memset(&g_input_state, 0, sizeof(InputState));
    g_prev_buttons = 0;
    g_mapping_count = sizeof(g_default_mapping) / sizeof(ButtonMapping);
    
    // Copy default mapping
    memcpy(g_current_mapping, g_default_mapping, sizeof(g_default_mapping));
    
    // Try to load custom mapping from SD
    input_3ds_load_mapping("/3ds/mb64/mapping.cfg");
}

void input_3ds_update(void) {
    hidScanInput();
    
    // Update button states
    u32 keys_down = hidKeysDown();
    u32 keys_held = hidKeysHeld();
    u32 keys_up = hidKeysUp();
    
    g_input_state.buttons_pressed = keys_down;
    g_input_state.buttons_held = keys_held;
    g_input_state.buttons_released = keys_up;
    
    // Read Circle Pad (main analog stick)
    circlePosition pos;
    hidCircleRead(&pos);
    
    // Convert to N64 range (-80 to 80 typically, we'll use -128 to 127)
    g_input_state.stick.x = (s8)(pos.dx / 160);  // Scale down
    g_input_state.stick.y = (s8)(pos.dy / 160);
    
    // Clamp values
    if (g_input_state.stick.x > 80) g_input_state.stick.x = 80;
    if (g_input_state.stick.x < -80) g_input_state.stick.x = -80;
    if (g_input_state.stick.y > 80) g_input_state.stick.y = 80;
    if (g_input_state.stick.y < -80) g_input_state.stick.y = -80;
    
    // Read C-Stick (n3DS only)
    circlePosition cstick;
    hidCstickRead(&cstick);
    g_input_state.cstick.x = (s8)(cstick.dx / 160);
    g_input_state.cstick.y = (s8)(cstick.dy / 160);
    
    // Read touch screen
    touchPosition touch;
    hidTouchRead(&touch);
    g_input_state.touch = touch;
    
    g_input_state.touch_pressed = (touch.px != 0 && g_prev_touch.px == 0);
    g_input_state.touch_held = (touch.px != 0);
    g_input_state.touch_released = (touch.px == 0 && g_prev_touch.px != 0);
    
    g_prev_touch = touch;
    g_prev_buttons = keys_held;
    
    // Add analog stick directions to buttons for compatibility
    if (g_input_state.stick.y > 40) keys_held |= KEY_CPAD_UP;
    if (g_input_state.stick.y < -40) keys_held |= KEY_CPAD_DOWN;
    if (g_input_state.stick.x < -40) keys_held |= KEY_CPAD_LEFT;
    if (g_input_state.stick.x > 40) keys_held |= KEY_CPAD_RIGHT;
    
    // Add C-stick to buttons
    if (g_input_state.cstick.y > 40) keys_held |= KEY_CSTICK_UP;
    if (g_input_state.cstick.y < -40) keys_held |= KEY_CSTICK_DOWN;
    if (g_input_state.cstick.x < -40) keys_held |= KEY_CSTICK_LEFT;
    if (g_input_state.cstick.x > 40) keys_held |= KEY_CSTICK_RIGHT;
}

const InputState* input_3ds_get_state(void) {
    return &g_input_state;
}

u32 input_3ds_get_n64_buttons(void) {
    u32 n64_buttons = 0;
    u32 keys_held = g_input_state.buttons_held;
    
    // Map 3DS buttons to N64 buttons
    for (int i = 0; i < g_mapping_count; i++) {
        if (keys_held & g_current_mapping[i].btn3ds_standard) {
            n64_buttons |= g_current_mapping[i].n64_button;
        }
        if (keys_held & g_current_mapping[i].btn3ds_alt) {
            n64_buttons |= g_current_mapping[i].n64_button;
        }
    }
    
    return n64_buttons;
}

void input_3ds_get_n64_stick(s8 *x, s8 *y) {
    *x = g_input_state.stick.x;
    *y = g_input_state.stick.y;
}

void input_3ds_load_mapping(const char *config_file) {
    FILE *f = fopen(config_file, "r");
    if (!f) {
        // No custom mapping, use defaults
        return;
    }
    
    // Simple config file format:
    // n64_button = 3ds_button
    // Lines starting with # are comments
    
    char line[256];
    int idx = 0;
    
    while (fgets(line, sizeof(line), f) && idx < 16) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;
        
        u32 n64_btn, btn3ds;
        if (sscanf(line, "%x = %x", &n64_btn, &btn3ds) == 2) {
            g_current_mapping[idx].n64_button = n64_btn;
            g_current_mapping[idx].btn3ds_standard = btn3ds;
            g_current_mapping[idx].btn3ds_alt = 0;
            idx++;
        }
    }
    
    if (idx > 0) {
        g_mapping_count = idx;
    }
    
    fclose(f);
}

void input_3ds_save_mapping(const char *config_file) {
    FILE *f = fopen(config_file, "w");
    if (!f) return;
    
    fprintf(f, "# Mario Builder 64 - 3DS Button Mapping\n");
    fprintf(f, "# Format: N64_BUTTON_HEX = 3DS_BUTTON_HEX\n\n");
    
    for (int i = 0; i < g_mapping_count; i++) {
        fprintf(f, "%04X = %08X\n", 
                g_current_mapping[i].n64_button,
                g_current_mapping[i].btn3ds_standard);
    }
    
    fclose(f);
}

void input_3ds_set_mapping(u32 n64_button, u32 btn3ds) {
    // Find existing mapping
    for (int i = 0; i < g_mapping_count; i++) {
        if (g_current_mapping[i].n64_button == n64_button) {
            g_current_mapping[i].btn3ds_standard = btn3ds;
            return;
        }
    }
    
    // Add new mapping if space available
    if (g_mapping_count < 16) {
        g_current_mapping[g_mapping_count].n64_button = n64_button;
        g_current_mapping[g_mapping_count].btn3ds_standard = btn3ds;
        g_current_mapping[g_mapping_count].btn3ds_alt = 0;
        g_mapping_count++;
    }
}

u8 input_3ds_touch_in_rect(u16 x, u16 y, u16 w, u16 h) {
    if (!g_input_state.touch_held) return 0;
    
    touchPosition *touch = &g_input_state.touch;
    return (touch->px >= x && touch->px < x + w &&
            touch->py >= y && touch->py < y + h);
}

u8 input_3ds_touch_pressed_in_rect(u16 x, u16 y, u16 w, u16 h) {
    if (!g_input_state.touch_pressed) return 0;
    
    touchPosition *touch = &g_input_state.touch;
    return (touch->px >= x && touch->px < x + w &&
            touch->py >= y && touch->py < y + h);
}

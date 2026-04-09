#ifndef _3DS_TOUCH_UI_H
#define _3DS_TOUCH_UI_H

#include "platform_3ds.h"
#include <citro3d.h>

// Touch screen UI for Mario Builder 64
// Provides 2D interface on bottom screen for builder mode

// UI Element types
typedef enum {
    UI_ELEMENT_BUTTON,
    UI_ELEMENT_SLIDER,
    UI_ELEMENT_PALETTE,
    UI_ELEMENT_GRID,
    UI_ELEMENT_LABEL,
    UI_ELEMENT_PANEL
} UIElementType;

// Touch UI element
typedef struct UIElement {
    UIElementType type;
    s16 x, y;           // Position
    s16 width, height;  // Size
    u32 color;          // Base color
    u32 hoverColor;     // Hover/pressed color
    u8 visible;         // Visibility
    u8 enabled;       // Interactivity
    u8 pressed;       // Current press state
    u8 wasPressed;    // Previous press state (for click detection)
    
    // Callbacks
    void (*onPress)(struct UIElement* elem, s16 x, s16 y);
    void (*onRelease)(struct UIElement* elem, s16 x, s16 y);
    void (*onDrag)(struct UIElement* elem, s16 x, s16 y);
    
    // Data pointer for custom elements
    void *data;
    
    // Linked list
    struct UIElement *next;
} UIElement;

// Builder tool types
typedef enum {
    TOOL_NONE,
    TOOL_SELECT,
    TOOL_BRUSH,
    TOOL_ERASER,
    TOOL_FILL,
    TOOL_EYEDROPPER,
    TOOL_STAMP,
    TOOL_COUNT
} BuilderTool;

// Block/Tile palette
typedef struct {
    u16 selectedBlock;
    u16 blockCount;
    u8 scrollOffset;
    u8 maxVisible;
} BlockPalette;

// Builder UI state
typedef struct {
    UIElement *elements;
    UIElement *pressedElement;
    
    BuilderTool currentTool;
    BlockPalette palette;
    
    s16 touchX, touchY;
    u8 touchPressed;
    u8 touchWasPressed;
    
    // UI regions
    s16 toolPanelX, toolPanelY;
    s16 toolPanelW, toolPanelH;
    
    s16 palettePanelX, palettePanelY;
    s16 palettePanelW, palettePanelH;
    
    s16 minimapX, minimapY;
    s16 minimapW, minimapH;
    
    // Minimap state
    u8 minimapScale;
    s16 minimapScrollX, minimapScrollY;
    
    // Quick select slots
    u16 quickSelect[8];
    u8 quickSelectActive;
    
} BuilderUI3DS;

// Initialize touch UI
void touch_ui_init(void);
void touch_ui_cleanup(void);

// Update and render
void touch_ui_update(void);
void touch_ui_render(void);

// Input handling
void touch_ui_handle_touch(s16 x, s16 y, u8 pressed);
void touch_ui_handle_drag(s16 x, s16 y);
void touch_ui_handle_release(s16 x, s16 y);

// Element management
UIElement* touch_ui_create_element(UIElementType type, s16 x, s16 y, s16 w, s16 h);
void touch_ui_destroy_element(UIElement *elem);
void touch_ui_destroy_all_elements(void);
UIElement* touch_ui_find_element_at(s16 x, s16 y);

// Builder UI specific
void touch_ui_builder_init(void);
void touch_ui_builder_update(void);
void touch_ui_builder_render(void);

void touch_ui_set_tool(BuilderTool tool);
BuilderTool touch_ui_get_tool(void);

void touch_ui_select_block(u16 blockId);
u16 touch_ui_get_selected_block(void);

// Tool callbacks
void touch_ui_on_tool_select(UIElement *elem, s16 x, s16 y);
void touch_ui_on_tool_brush(UIElement *elem, s16 x, s16 y);
void touch_ui_on_tool_eraser(UIElement *elem, s16 x, s16 y);
void touch_ui_on_tool_fill(UIElement *elem, s16 x, s16 y);
void touch_ui_on_tool_eyedropper(UIElement *elem, s16 x, s16 y);

// Palette callbacks
void touch_ui_on_palette_scroll(UIElement *elem, s16 x, s16 y);
void touch_ui_on_block_select(UIElement *elem, s16 x, s16 y);

// Minimap rendering
void touch_ui_render_minimap(s16 x, s16 y, s16 w, s16 h);
void touch_ui_minimap_set_level(void *levelData);

// Drawing functions (2D)
void touch_ui_draw_rect(s16 x, s16 y, s16 w, s16 h, u32 color);
void touch_ui_draw_rect_outline(s16 x, s16 y, s16 w, s16 h, u32 color, s16 thickness);
void touch_ui_draw_circle(s16 x, s16 y, s16 r, u32 color);
void touch_ui_draw_line(s16 x1, s16 y1, s16 x2, s16 y2, u32 color, s16 thickness);
void touch_ui_draw_text(s16 x, s16 y, const char *text, u32 color);
void touch_ui_draw_icon(s16 x, s16 y, u32 iconId, u32 color);

// Panel drawing
void touch_ui_draw_panel(s16 x, s16 y, s16 w, s16 h, const char *title);
void touch_ui_draw_button(s16 x, s16 y, s16 w, s16 h, const char *label, u8 pressed);
void touch_ui_draw_palette(s16 x, s16 y, s16 w, s16 h, BlockPalette *palette);
void touch_ui_draw_slider(s16 x, s16 y, s16 w, s32 min, s32 max, s32 value);

// Global state
BuilderUI3DS* touch_ui_get_builder(void);

#endif // _3DS_TOUCH_UI_H

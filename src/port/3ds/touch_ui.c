#include "touch_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static BuilderUI3DS g_builder_ui;

void touch_ui_init(void) {
    memset(&g_builder_ui, 0, sizeof(BuilderUI3DS));
    
    // Set up default panel positions for 320x240 bottom screen
    g_builder_ui.toolPanelX = 0;
    g_builder_ui.toolPanelY = 0;
    g_builder_ui.toolPanelW = 60;
    g_builder_ui.toolPanelH = 240;
    
    g_builder_ui.palettePanelX = 260;
    g_builder_ui.palettePanelY = 0;
    g_builder_ui.palettePanelW = 60;
    g_builder_ui.palettePanelH = 200;
    
    g_builder_ui.minimapX = 60;
    g_builder_ui.minimapY = 0;
    g_builder_ui.minimapW = 200;
    g_builder_ui.minimapH = 180;
    
    g_builder_ui.palette.maxVisible = 10;
    g_builder_ui.minimapScale = 2;
}

void touch_ui_cleanup(void) {
    touch_ui_destroy_all_elements();
}

void touch_ui_builder_init(void) {
    touch_ui_init();
    
    // Create tool buttons
    UIElement *selectBtn = touch_ui_create_element(UI_ELEMENT_BUTTON, 5, 10, 50, 40);
    selectBtn->color = 0xFF808080;
    selectBtn->hoverColor = 0xFF606060;
    selectBtn->onPress = touch_ui_on_tool_select;
    selectBtn->data = (void*)TOOL_SELECT;
    
    UIElement *brushBtn = touch_ui_create_element(UI_ELEMENT_BUTTON, 5, 55, 50, 40);
    brushBtn->color = 0xFF808080;
    brushBtn->hoverColor = 0xFF606060;
    brushBtn->onPress = touch_ui_on_tool_brush;
    brushBtn->data = (void*)TOOL_BRUSH;
    
    UIElement *eraserBtn = touch_ui_create_element(UI_ELEMENT_BUTTON, 5, 100, 50, 40);
    eraserBtn->color = 0xFF808080;
    eraserBtn->hoverColor = 0xFF606060;
    eraserBtn->onPress = touch_ui_on_tool_eraser;
    eraserBtn->data = (void*)TOOL_ERASER;
    
    UIElement *fillBtn = touch_ui_create_element(UI_ELEMENT_BUTTON, 5, 145, 50, 40);
    fillBtn->color = 0xFF808080;
    fillBtn->hoverColor = 0xFF606060;
    fillBtn->onPress = touch_ui_on_tool_fill;
    fillBtn->data = (void*)TOOL_FILL;
    
    // Create palette panel
    UIElement *palettePanel = touch_ui_create_element(UI_ELEMENT_PALETTE, 
        g_builder_ui.palettePanelX, g_builder_ui.palettePanelY,
        g_builder_ui.palettePanelW, g_builder_ui.palettePanelH);
    palettePanel->color = 0xFF404040;
    
    // Set initial tool
    g_builder_ui.currentTool = TOOL_BRUSH;
}

void touch_ui_builder_update(void) {
    // Update UI elements
    touch_ui_update();
}

void touch_ui_builder_render(void) {
    // Clear bottom screen area
    touch_ui_draw_rect(0, 0, 320, 240, 0xFF202020);
    
    // Draw tool panel
    touch_ui_draw_panel(g_builder_ui.toolPanelX, g_builder_ui.toolPanelY,
                       g_builder_ui.toolPanelW, g_builder_ui.toolPanelH, "Tools");
    
    // Draw tool buttons
    u32 toolColors[TOOL_COUNT] = {
        0xFF808080, // NONE
        0xFFFFD700, // SELECT (gold)
        0xFF4169E1, // BRUSH (royal blue)
        0xFFFF6347, // ERASER (tomato)
        0xFF32CD32, // FILL (lime green)
        0xFF9932CC, // EYEDROPPER (orchid)
        0xFFFF69B4, // STAMP (hot pink)
    };
    
    const char *toolLabels[TOOL_COUNT] = {
        "", "Sel", "Brsh", "Ersr", "Fill", "Drop", "Stmp"
    };
    
    for (int i = 1; i < TOOL_COUNT; i++) {
        s16 y = 10 + (i - 1) * 45;
        u8 pressed = (g_builder_ui.currentTool == i);
        touch_ui_draw_button(5, y, 50, 40, toolLabels[i], pressed);
    }
    
    // Draw minimap
    touch_ui_render_minimap(g_builder_ui.minimapX, g_builder_ui.minimapY,
                           g_builder_ui.minimapW, g_builder_ui.minimapH);
    
    // Draw palette panel
    touch_ui_draw_panel(g_builder_ui.palettePanelX, g_builder_ui.palettePanelY,
                       g_builder_ui.palettePanelW, g_builder_ui.palettePanelH, "Blocks");
    
    touch_ui_draw_palette(g_builder_ui.palettePanelX + 5, g_builder_ui.palettePanelY + 25,
                         g_builder_ui.palettePanelW - 10, g_builder_ui.palettePanelH - 30,
                         &g_builder_ui.palette);
    
    // Draw current selection info
    char infoText[64];
    snprintf(infoText, sizeof(infoText), "Tool: %s", 
             g_builder_ui.currentTool == TOOL_SELECT ? "Select" :
             g_builder_ui.currentTool == TOOL_BRUSH ? "Brush" :
             g_builder_ui.currentTool == TOOL_ERASER ? "Eraser" :
             g_builder_ui.currentTool == TOOL_FILL ? "Fill" : "None");
    
    touch_ui_draw_text(10, 230, infoText, 0xFFFFFFFF);
}

void touch_ui_update(void) {
    // Update all UI elements
    UIElement *elem = g_builder_ui.elements;
    while (elem) {
        if (elem->pressed && elem->onDrag) {
            elem->onDrag(elem, g_builder_ui.touchX, g_builder_ui.touchY);
        }
        elem = elem->next;
    }
}

void touch_ui_render(void) {
    // Generic UI render - used by builder_render
}

void touch_ui_handle_touch(s16 x, s16 y, u8 pressed) {
    g_builder_ui.touchX = x;
    g_builder_ui.touchY = y;
    g_builder_ui.touchPressed = pressed;
    
    if (pressed && !g_builder_ui.touchWasPressed) {
        // New press - find element
        UIElement *elem = touch_ui_find_element_at(x, y);
        if (elem && elem->enabled && elem->onPress) {
            elem->pressed = 1;
            g_builder_ui.pressedElement = elem;
            elem->onPress(elem, x, y);
        }
    } else if (!pressed && g_builder_ui.touchWasPressed) {
        // Release
        if (g_builder_ui.pressedElement) {
            if (g_builder_ui.pressedElement->onRelease) {
                g_builder_ui.pressedElement->onRelease(g_builder_ui.pressedElement, x, y);
            }
            g_builder_ui.pressedElement->pressed = 0;
            g_builder_ui.pressedElement = NULL;
        }
    }
    
    g_builder_ui.touchWasPressed = pressed;
}

void touch_ui_handle_drag(s16 x, s16 y) {
    if (g_builder_ui.pressedElement && g_builder_ui.pressedElement->onDrag) {
        g_builder_ui.pressedElement->onDrag(g_builder_ui.pressedElement, x, y);
    }
}

void touch_ui_handle_release(s16 x, s16 y) {
    touch_ui_handle_touch(x, y, 0);
}

UIElement* touch_ui_create_element(UIElementType type, s16 x, s16 y, s16 w, s16 h) {
    UIElement *elem = (UIElement *)malloc(sizeof(UIElement));
    if (!elem) return NULL;
    
    memset(elem, 0, sizeof(UIElement));
    elem->type = type;
    elem->x = x;
    elem->y = y;
    elem->width = w;
    elem->height = h;
    elem->color = 0xFF808080;
    elem->hoverColor = 0xFF606060;
    elem->visible = 1;
    elem->enabled = 1;
    
    // Add to list
    elem->next = g_builder_ui.elements;
    g_builder_ui.elements = elem;
    
    return elem;
}

void touch_ui_destroy_element(UIElement *elem) {
    if (!elem) return;
    
    // Remove from list
    UIElement **current = &g_builder_ui.elements;
    while (*current) {
        if (*current == elem) {
            *current = elem->next;
            free(elem);
            return;
        }
        current = &(*current)->next;
    }
}

void touch_ui_destroy_all_elements(void) {
    while (g_builder_ui.elements) {
        UIElement *next = g_builder_ui.elements->next;
        free(g_builder_ui.elements);
        g_builder_ui.elements = next;
    }
}

UIElement* touch_ui_find_element_at(s16 x, s16 y) {
    UIElement *elem = g_builder_ui.elements;
    while (elem) {
        if (elem->visible && 
            x >= elem->x && x < elem->x + elem->width &&
            y >= elem->y && y < elem->y + elem->height) {
            return elem;
        }
        elem = elem->next;
    }
    return NULL;
}

// Tool callbacks
void touch_ui_on_tool_select(UIElement *elem, s16 x, s16 y) {
    g_builder_ui.currentTool = TOOL_SELECT;
}

void touch_ui_on_tool_brush(UIElement *elem, s16 x, s16 y) {
    g_builder_ui.currentTool = TOOL_BRUSH;
}

void touch_ui_on_tool_eraser(UIElement *elem, s16 x, s16 y) {
    g_builder_ui.currentTool = TOOL_ERASER;
}

void touch_ui_on_tool_fill(UIElement *elem, s16 x, s16 y) {
    g_builder_ui.currentTool = TOOL_FILL;
}

void touch_ui_on_tool_eyedropper(UIElement *elem, s16 x, s16 y) {
    g_builder_ui.currentTool = TOOL_EYEDROPPER;
}

void touch_ui_on_palette_scroll(UIElement *elem, s16 x, s16 y) {
    // Scroll palette
}

void touch_ui_on_block_select(UIElement *elem, s16 x, s16 y) {
    // Select block from palette
    s16 localY = y - elem->y;
    s16 blockIndex = localY / 20 + g_builder_ui.palette.scrollOffset;
    if (blockIndex < g_builder_ui.palette.blockCount) {
        g_builder_ui.palette.selectedBlock = blockIndex;
    }
}

void touch_ui_set_tool(BuilderTool tool) {
    g_builder_ui.currentTool = tool;
}

BuilderTool touch_ui_get_tool(void) {
    return g_builder_ui.currentTool;
}

void touch_ui_select_block(u16 blockId) {
    g_builder_ui.palette.selectedBlock = blockId;
}

u16 touch_ui_get_selected_block(void) {
    return g_builder_ui.palette.selectedBlock;
}

void touch_ui_render_minimap(s16 x, s16 y, s16 w, s16 h) {
    // Draw minimap background
    touch_ui_draw_rect(x, y, w, h, 0xFF101010);
    touch_ui_draw_rect_outline(x, y, w, h, 0xFF606060, 2);
    
    // Draw placeholder level representation
    // In real implementation, this would render the actual level
    touch_ui_draw_rect(x + w/4, y + h/4, w/2, h/2, 0xFF404040);
    
    // Draw player position marker
    s16 playerX = x + w/2;
    s16 playerY = y + h/2;
    touch_ui_draw_circle(playerX, playerY, 4, 0xFFFF0000);
}

void touch_ui_minimap_set_level(void *levelData) {
    // Set level data for minimap rendering
}

// Drawing functions (placeholder implementations)
void touch_ui_draw_rect(s16 x, s16 y, s16 w, s16 h, u32 color) {
    // Would use C3D or software rendering to draw filled rectangle
}

void touch_ui_draw_rect_outline(s16 x, s16 y, s16 w, s16 h, u32 color, s16 thickness) {
    // Draw rectangle outline
}

void touch_ui_draw_circle(s16 x, s16 y, s16 r, u32 color) {
    // Draw filled circle
}

void touch_ui_draw_line(s16 x1, s16 y1, s16 x2, s16 y2, u32 color, s16 thickness) {
    // Draw line
}

void touch_ui_draw_text(s16 x, s16 y, const char *text, u32 color) {
    // Draw text using font
}

void touch_ui_draw_icon(s16 x, s16 y, u32 iconId, u32 color) {
    // Draw icon
}

void touch_ui_draw_panel(s16 x, s16 y, s16 w, s16 h, const char *title) {
    // Panel background
    touch_ui_draw_rect(x, y, w, h, 0xFF404040);
    touch_ui_draw_rect_outline(x, y, w, h, 0xFF606060, 1);
    
    // Title bar
    touch_ui_draw_rect(x, y, w, 20, 0xFF606060);
    if (title) {
        touch_ui_draw_text(x + 5, y + 5, title, 0xFFFFFFFF);
    }
}

void touch_ui_draw_button(s16 x, s16 y, s16 w, s16 h, const char *label, u8 pressed) {
    u32 bgColor = pressed ? 0xFF404040 : 0xFF606060;
    u32 borderColor = pressed ? 0xFFFFFFFF : 0xFF808080;
    
    touch_ui_draw_rect(x, y, w, h, bgColor);
    touch_ui_draw_rect_outline(x, y, w, h, borderColor, 2);
    
    if (label) {
        s16 textX = x + w/2 - 12; // Center roughly
        s16 textY = y + h/2 - 8;
        touch_ui_draw_text(textX, textY, label, 0xFFFFFFFF);
    }
}

void touch_ui_draw_palette(s16 x, s16 y, s16 w, s16 h, BlockPalette *palette) {
    s16 blockSize = 40;
    s16 spacing = 2;
    
    for (int i = 0; i < palette->maxVisible && (palette->scrollOffset + i) < palette->blockCount; i++) {
        u16 blockId = palette->scrollOffset + i;
        s16 blockY = y + i * (blockSize + spacing);
        
        // Block preview color (would be actual block texture)
        u32 blockColor = 0xFF808080 + (blockId * 0x101010);
        
        // Selection highlight
        if (blockId == palette->selectedBlock) {
            touch_ui_draw_rect_outline(x - 2, blockY - 2, blockSize + 4, blockSize + 4, 0xFFFFFF00, 2);
        }
        
        touch_ui_draw_rect(x, blockY, blockSize, blockSize, blockColor);
        touch_ui_draw_rect_outline(x, blockY, blockSize, blockSize, 0xFF404040, 1);
    }
}

void touch_ui_draw_slider(s16 x, s16 y, s16 w, s32 min, s32 max, s32 value) {
    // Slider track
    touch_ui_draw_rect(x, y + 6, w, 8, 0xFF404040);
    
    // Slider handle
    s32 range = max - min;
    s32 pos = ((value - min) * (w - 20)) / range;
    touch_ui_draw_rect(x + pos, y, 20, 20, 0xFF606060);
    touch_ui_draw_rect_outline(x + pos, y, 20, 20, 0xFFFFFFFF, 1);
}

BuilderUI3DS* touch_ui_get_builder(void) {
    return &g_builder_ui;
}

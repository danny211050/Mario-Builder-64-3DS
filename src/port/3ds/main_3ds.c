#include <3ds.h>
#include <citro3d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform_3ds.h"
#include "n64_3ds_adapter.h"

int main(int argc, char *argv[]) {
    // Initialize 3DS
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);
    romfsInit();
    
    printf("Mario Builder 64 - 3DS Port\n");
    printf("============================\n");
    printf("Initializing...\n\n");
    
    // Run the game via N64 adapter
    n64_3ds_main();
    
    printf("\nGame exited.\n");
    
    // Cleanup
    romfsExit();
    gfxExit();
    
    return 0;
}

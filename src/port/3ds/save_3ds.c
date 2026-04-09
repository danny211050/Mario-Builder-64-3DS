#include "save_3ds.h"
#include <string.h>
#include <stdio.h>

static SaveData3DS g_save_data;
static bool g_save_initialized = false;

// N64 EEPROM is typically 4Kb (512 bytes) or 16Kb (2048 bytes)
// We use a larger buffer to support both
#define EEPROM_SIZE 2048

void save_3ds_init(void) {
    if (g_save_initialized) return;
    
    memset(&g_save_data, 0, sizeof(SaveData3DS));
    g_save_data.version = 1;
    
    // Ensure save directory exists
    FS_Archive archive;
    FS_Path path;
    Handle dirHandle;
    
    // Create /3ds/mb64 directory if needed
    Result res = FSUSER_OpenArchive(&archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
    if (R_SUCCEEDED(res)) {
        FSUSER_CreateDirectory(archive, fsMakePath(PATH_ASCII, "/3ds/mb64"), FS_ATTRIBUTE_DIRECTORY);
        FSUSER_CloseArchive(archive);
    }
    
    // Try to load existing save
    if (save_3ds_load_file() != 0) {
        // No save file found, initialize with defaults
        memset(g_save_data.data, 0, SAVE_SIZE);
        g_save_data.checksum = save_3ds_calculate_checksum(g_save_data.data, SAVE_SIZE);
        save_3ds_save_file();
    }
    
    g_save_initialized = true;
}

int save_3ds_read(void *buffer, u32 size) {
    if (!g_save_initialized) save_3ds_init();
    if (size > SAVE_SIZE) size = SAVE_SIZE;
    
    memcpy(buffer, g_save_data.data, size);
    return size;
}

int save_3ds_write(const void *buffer, u32 size) {
    if (!g_save_initialized) save_3ds_init();
    if (size > SAVE_SIZE) size = SAVE_SIZE;
    
    memcpy(g_save_data.data, buffer, size);
    g_save_data.checksum = save_3ds_calculate_checksum(g_save_data.data, SAVE_SIZE);
    
    return save_3ds_save_file();
}

// EEPROM byte access
u8 eeprom_3ds_read_byte(u16 addr) {
    if (addr >= EEPROM_SIZE) return 0;
    if (!g_save_initialized) save_3ds_init();
    
    return g_save_data.data[addr];
}

void eeprom_3ds_write_byte(u16 addr, u8 value) {
    if (addr >= EEPROM_SIZE) return;
    if (!g_save_initialized) save_3ds_init();
    
    g_save_data.data[addr] = value;
}

int eeprom_3ds_read_array(u16 addr, u8 *buffer, u16 len) {
    if (addr >= EEPROM_SIZE) return -1;
    if (addr + len > EEPROM_SIZE) len = EEPROM_SIZE - addr;
    if (!g_save_initialized) save_3ds_init();
    
    memcpy(buffer, &g_save_data.data[addr], len);
    return len;
}

int eeprom_3ds_write_array(u16 addr, const u8 *buffer, u16 len) {
    if (addr >= EEPROM_SIZE) return -1;
    if (addr + len > EEPROM_SIZE) len = EEPROM_SIZE - addr;
    if (!g_save_initialized) save_3ds_init();
    
    memcpy(&g_save_data.data[addr], buffer, len);
    return len;
}

// SRAM emulation (for larger save types)
static u8 g_sram_data[0x8000]; // 32KB SRAM

int sram_3ds_read(void *buffer, u32 size, u32 offset) {
    if (offset >= sizeof(g_sram_data)) return -1;
    if (offset + size > sizeof(g_sram_data)) size = sizeof(g_sram_data) - offset;
    
    memcpy(buffer, &g_sram_data[offset], size);
    return size;
}

int sram_3ds_write(const void *buffer, u32 size, u32 offset) {
    if (offset >= sizeof(g_sram_data)) return -1;
    if (offset + size > sizeof(g_sram_data)) size = sizeof(g_sram_data) - offset;
    
    memcpy(&g_sram_data[offset], buffer, size);
    return size;
}

// File operations
int save_3ds_load_file(void) {
    FILE *f = fopen(SAVE_FILENAME, "rb");
    if (!f) return -1;
    
    size_t read = fread(&g_save_data, 1, sizeof(SaveData3DS), f);
    fclose(f);
    
    if (read != sizeof(SaveData3DS)) {
        // Try to load old format (just raw data)
        f = fopen(SAVE_FILENAME, "rb");
        if (f) {
            memset(&g_save_data, 0, sizeof(SaveData3DS));
            fread(g_save_data.data, 1, SAVE_SIZE, f);
            fclose(f);
            g_save_data.checksum = save_3ds_calculate_checksum(g_save_data.data, SAVE_SIZE);
        }
    }
    
    // Verify checksum
    if (!save_3ds_verify_checksum()) {
        // Try to restore from backup
        save_3ds_restore_backup();
    }
    
    return 0;
}

int save_3ds_save_file(void) {
    // Create backup of current save first
    save_3ds_create_backup();
    
    FILE *f = fopen(SAVE_FILENAME, "wb");
    if (!f) return -1;
    
    g_save_data.checksum = save_3ds_calculate_checksum(g_save_data.data, SAVE_SIZE);
    
    size_t written = fwrite(&g_save_data, 1, sizeof(SaveData3DS), f);
    fclose(f);
    
    return (written == sizeof(SaveData3DS)) ? 0 : -1;
}

void save_3ds_create_backup(void) {
    // Copy current save to backup
    FILE *src = fopen(SAVE_FILENAME, "rb");
    if (!src) return;
    
    FILE *dst = fopen(SAVE_BACKUP_FILENAME, "wb");
    if (!dst) {
        fclose(src);
        return;
    }
    
    int c;
    while ((c = fgetc(src)) != EOF) {
        fputc(c, dst);
    }
    
    fclose(src);
    fclose(dst);
}

void save_3ds_restore_backup(void) {
    FILE *src = fopen(SAVE_BACKUP_FILENAME, "rb");
    if (!src) return;
    
    FILE *dst = fopen(SAVE_FILENAME, "wb");
    if (!dst) {
        fclose(src);
        return;
    }
    
    int c;
    while ((c = fgetc(src)) != EOF) {
        fputc(c, dst);
    }
    
    fclose(src);
    fclose(dst);
    
    // Reload the restored save
    save_3ds_load_file();
}

u32 save_3ds_calculate_checksum(const void *data, u32 size) {
    const u8 *bytes = (const u8 *)data;
    u32 checksum = 0;
    
    for (u32 i = 0; i < size; i++) {
        checksum = checksum * 31 + bytes[i];
    }
    
    return checksum;
}

bool save_3ds_verify_checksum(void) {
    u32 calc_checksum = save_3ds_calculate_checksum(g_save_data.data, SAVE_SIZE);
    return (calc_checksum == g_save_data.checksum);
}

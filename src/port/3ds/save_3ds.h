#ifndef _3DS_SAVE_H
#define _3DS_SAVE_H

#include <3ds.h>
#include "platform_3ds.h"

// Save data configuration
#define SAVE_FILENAME "/3ds/mb64/save.bin"
#define SAVE_BACKUP_FILENAME "/3ds/mb64/save_backup.bin"
#define SAVE_SIZE 0x2000  // 8KB like N64 EEPROM

// Save data structure matching N64 format
typedef struct {
    u8 data[SAVE_SIZE];
    u32 checksum;
    u32 version;
    u32 flags;
} SaveData3DS;

// Initialize save system
void save_3ds_init(void);
void save_3ds_cleanup(void);

// Read/write save data
int save_3ds_read(void *buffer, u32 size);
int save_3ds_write(const void *buffer, u32 size);

// EEPROM emulation (for N64 compatibility)
u8 eeprom_3ds_read_byte(u16 addr);
void eeprom_3ds_write_byte(u16 addr, u8 value);
int eeprom_3ds_read_array(u16 addr, u8 *buffer, u16 len);
int eeprom_3ds_write_array(u16 addr, const u8 *buffer, u16 len);

// SRAM/FlashRAM emulation (if needed)
int sram_3ds_read(void *buffer, u32 size, u32 offset);
int sram_3ds_write(const void *buffer, u32 size, u32 offset);

// Save file management
int save_3ds_load_file(void);
int save_3ds_save_file(void);
void save_3ds_create_backup(void);
void save_3ds_restore_backup(void);

// Save integrity
u32 save_3ds_calculate_checksum(const void *data, u32 size);
bool save_3ds_verify_checksum(void);

#endif // _3DS_SAVE_H

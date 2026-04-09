#ifndef _3DS_ROMFS_H
#define _3DS_ROMFS_H

#include <3ds.h>
#include <stdio.h>

// ROMFS file system adapter
// Provides stdio-compatible file access for game assets

// Initialize ROMFS
int romfs_3ds_init(void);
void romfs_3ds_exit(void);

// File operations
FILE* romfs_3ds_fopen(const char *path, const char *mode);
size_t romfs_3ds_fread(void *ptr, size_t size, size_t count, FILE *stream);
size_t romfs_3ds_fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
int romfs_3ds_fseek(FILE *stream, long offset, int origin);
long romfs_3ds_ftell(FILE *stream);
int romfs_3ds_fclose(FILE *stream);

// Directory operations
typedef struct {
    Handle handle;
    FS_DirectoryEntry entry;
    int has_more;
} RomfsDir;

RomfsDir* romfs_3ds_opendir(const char *path);
int romfs_3ds_readdir(RomfsDir *dir, char *name, size_t name_size);
void romfs_3ds_closedir(RomfsDir *dir);

// File existence and info
int romfs_3ds_exists(const char *path);
u64 romfs_3ds_file_size(const char *path);

// Path utilities
void romfs_3ds_set_base_path(const char *path);
const char* romfs_3ds_get_full_path(const char *relative_path);

#endif // _3DS_ROMFS_H

#include "romfs_3ds.h"
#include <string.h>
#include <stdlib.h>

static char g_base_path[256] = "romfs:/";
static int g_romfs_initialized = 0;

int romfs_3ds_init(void) {
    if (g_romfs_initialized) return 0;
    
    Result res = romfsInit();
    if (R_FAILED(res)) {
        printf("Failed to initialize ROMFS: %08lX\n", res);
        return -1;
    }
    
    g_romfs_initialized = 1;
    return 0;
}

void romfs_3ds_exit(void) {
    if (g_romfs_initialized) {
        romfsExit();
        g_romfs_initialized = 0;
    }
}

FILE* romfs_3ds_fopen(const char *path, const char *mode) {
    char full_path[512];
    
    // Build full path
    if (path[0] == '/') {
        // Absolute path within romfs
        snprintf(full_path, sizeof(full_path), "romfs:%s", path);
    } else {
        // Relative path
        snprintf(full_path, sizeof(full_path), "%s%s", g_base_path, path);
    }
    
    return fopen(full_path, mode);
}

size_t romfs_3ds_fread(void *ptr, size_t size, size_t count, FILE *stream) {
    return fread(ptr, size, count, stream);
}

size_t romfs_3ds_fwrite(const void *ptr, size_t size, size_t count, FILE *stream) {
    return fwrite(ptr, size, count, stream);
}

int romfs_3ds_fseek(FILE *stream, long offset, int origin) {
    return fseek(stream, offset, origin);
}

long romfs_3ds_ftell(FILE *stream) {
    return ftell(stream);
}

int romfs_3ds_fclose(FILE *stream) {
    return fclose(stream);
}

RomfsDir* romfs_3ds_opendir(const char *path) {
    char full_path[512];
    
    if (path[0] == '/') {
        snprintf(full_path, sizeof(full_path), "romfs:%s", path);
    } else {
        snprintf(full_path, sizeof(full_path), "%s%s", g_base_path, path);
    }
    
    // Use FSUSER for directory access
    FS_Archive archive;
    Result res = FSUSER_OpenArchive(&archive, ARCHIVE_ROMFS, fsMakePath(PATH_EMPTY, ""));
    if (R_FAILED(res)) {
        return NULL;
    }
    
    RomfsDir *dir = (RomfsDir *)malloc(sizeof(RomfsDir));
    if (!dir) {
        FSUSER_CloseArchive(archive);
        return NULL;
    }
    
    FS_Path dir_path = fsMakePath(PATH_ASCII, full_path + 6); // Skip "romfs:"
    res = FSUSER_OpenDirectory(&dir->handle, archive, dir_path);
    FSUSER_CloseArchive(archive);
    
    if (R_FAILED(res)) {
        free(dir);
        return NULL;
    }
    
    dir->has_more = 1;
    return dir;
}

int romfs_3ds_readdir(RomfsDir *dir, char *name, size_t name_size) {
    if (!dir || !dir->has_more) return 0;
    
    u32 entries_read = 0;
    Result res = FSDIR_Read(dir->handle, &entries_read, 1, &dir->entry);
    
    if (R_FAILED(res) || entries_read == 0) {
        dir->has_more = 0;
        return 0;
    }
    
    // Convert UTF-16 name to ASCII
    u32 i;
    for (i = 0; i < 256 && i < name_size - 1; i++) {
        name[i] = (char)dir->entry.name[i];
    }
    name[i] = '\0';
    
    return 1;
}

void romfs_3ds_closedir(RomfsDir *dir) {
    if (dir) {
        FSDIR_Close(dir->handle);
        free(dir);
    }
}

int romfs_3ds_exists(const char *path) {
    char full_path[512];
    
    if (path[0] == '/') {
        snprintf(full_path, sizeof(full_path), "romfs:%s", path);
    } else {
        snprintf(full_path, sizeof(full_path), "%s%s", g_base_path, path);
    }
    
    FILE *f = fopen(full_path, "rb");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

u64 romfs_3ds_file_size(const char *path) {
    char full_path[512];
    
    if (path[0] == '/') {
        snprintf(full_path, sizeof(full_path), "romfs:%s", path);
    } else {
        snprintf(full_path, sizeof(full_path), "%s%s", g_base_path, path);
    }
    
    FILE *f = fopen(full_path, "rb");
    if (!f) return 0;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    
    return (u64)size;
}

void romfs_3ds_set_base_path(const char *path) {
    strncpy(g_base_path, path, sizeof(g_base_path) - 1);
    g_base_path[sizeof(g_base_path) - 1] = '\0';
    
    // Ensure trailing slash
    size_t len = strlen(g_base_path);
    if (len > 0 && g_base_path[len - 1] != '/') {
        if (len < sizeof(g_base_path) - 1) {
            g_base_path[len] = '/';
            g_base_path[len + 1] = '\0';
        }
    }
}

const char* romfs_3ds_get_full_path(const char *relative_path) {
    static char full_path[512];
    
    if (relative_path[0] == '/') {
        snprintf(full_path, sizeof(full_path), "romfs:%s", relative_path);
    } else {
        snprintf(full_path, sizeof(full_path), "%s%s", g_base_path, relative_path);
    }
    
    return full_path;
}

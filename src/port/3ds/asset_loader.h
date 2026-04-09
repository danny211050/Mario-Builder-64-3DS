#ifndef _3DS_ASSET_LOADER_H
#define _3DS_ASSET_LOADER_H

#include "PR/ultratypes.h"
#include "platform_3ds.h"
#include "gbi_3ds.h"
#include "romfs_3ds.h"

// ROMFS Asset Loader for Mario Builder 64
// Provides compatibility layer between N64 asset loading and 3DS ROMFS

// Asset types
typedef enum {
    ASSET_TYPE_TEXTURE,
    ASSET_TYPE_MODEL,
    ASSET_TYPE_LEVEL,
    ASSET_TYPE_SOUND,
    ASSET_TYPE_MUSIC,
    ASSET_TYPE_ANIMATION,
    ASSET_TYPE_FONT,
    ASSET_TYPE_DATA
} AssetType;

// Asset handle
typedef struct {
    AssetType type;
    char path[256];
    u32 size;
    void *data;
    u8 loaded;
} Asset3DS;

// Asset cache
typedef struct {
    Asset3DS *assets;
    u32 capacity;
    u32 count;
} AssetCache3DS;

// Level data structure (simplified)
typedef struct {
    u32 levelId;
    char name[32];
    void *geoData;
    void *collisionData;
    u32 geoSize;
    u32 collisionSize;
} LevelData3DS;

// N64 vertex format - defined in gbi_3ds.h
// typedef struct {
//     s16 pos[3];      // Position
//     s16 tc[2];       // Texture coordinates
//     u8 color[4];     // Vertex color
// } N64Vertex;

// Texture data
typedef struct {
    u32 format;     // N64 texture format
    u32 width;
    u32 height;
    u32 size;
    void *data;
} TextureData3DS;

// Model data
typedef struct {
    u32 vtxCount;
    u32 triCount;
    N64Vertex *vertices;
    u16 *indices;
} ModelData3DS;

// Initialize asset loader
void asset_loader_3ds_init(void);
void asset_loader_3ds_cleanup(void);

// Generic asset loading
Asset3DS* asset_loader_load(const char *path, AssetType type);
void asset_loader_unload(Asset3DS *asset);
void asset_loader_unload_all(void);

// Specific asset loading
TextureData3DS* asset_loader_load_texture(const char *path);
ModelData3DS* asset_loader_load_model(const char *path);
LevelData3DS* asset_loader_load_level(const char *path);
void* asset_loader_load_sound(const char *path, u32 *size);
void* asset_loader_load_music(const char *path, u32 *size);

// Level management
u32 asset_loader_get_level_count(void);
LevelData3DS* asset_loader_get_level(u32 index);
LevelData3DS* asset_loader_get_level_by_id(u32 levelId);

// Texture management
void asset_loader_register_texture(u32 texId, const char *path);
TextureData3DS* asset_loader_get_texture(u32 texId);
void asset_loader_bind_texture(u32 texId);

// Model management
void asset_loader_register_model(u32 modelId, const char *path);
ModelData3DS* asset_loader_get_model(u32 modelId);

// File utilities
u32 asset_loader_file_exists(const char *path);
u32 asset_loader_file_size(const char *path);
void* asset_loader_file_read(const char *path, u32 *size);

// N64 segment loading (for compatibility)
void asset_loader_load_segment(u32 segment, const char *path);
void asset_loader_unload_segment(u32 segment);
void* asset_loader_segment_to_virtual(u32 segment, u32 offset);

// Cache management
void asset_loader_set_cache_size(u32 maxAssets);
void asset_loader_clear_cache(void);
u32 asset_loader_get_cache_usage(void);

// Asset directories
void asset_loader_set_texture_dir(const char *dir);
void asset_loader_set_model_dir(const char *dir);
void asset_loader_set_level_dir(const char *dir);
void asset_loader_set_sound_dir(const char *dir);

// Global state
AssetCache3DS* asset_loader_get_cache(void);

#endif // _3DS_ASSET_LOADER_H

#include "asset_loader.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static AssetCache3DS g_asset_cache;
static char g_texture_dir[256] = "textures/";
static char g_model_dir[256] = "models/";
static char g_level_dir[256] = "levels/";
static char g_sound_dir[256] = "sounds/";

// Registered assets
static TextureData3DS *g_textures[1024];
static ModelData3DS *g_models[1024];

void asset_loader_3ds_init(void) {
    memset(&g_asset_cache, 0, sizeof(AssetCache3DS));
    memset(g_textures, 0, sizeof(g_textures));
    memset(g_models, 0, sizeof(g_models));
    
    // Initialize ROMFS
    romfs_3ds_init();
    
    printf("Asset loader initialized\n");
}

void asset_loader_3ds_cleanup(void) {
    asset_loader_unload_all();
    romfs_3ds_exit();
}

Asset3DS* asset_loader_load(const char *path, AssetType type) {
    // Check cache first
    for (u32 i = 0; i < g_asset_cache.count; i++) {
        if (strcmp(g_asset_cache.assets[i].path, path) == 0) {
            return &g_asset_cache.assets[i];
        }
    }
    
    // Load new asset
    if (g_asset_cache.count >= g_asset_cache.capacity) {
        printf("Asset cache full!\n");
        return NULL;
    }
    
    Asset3DS *asset = &g_asset_cache.assets[g_asset_cache.count++];
    strncpy(asset->path, path, sizeof(asset->path) - 1);
    asset->type = type;
    
    // Load data based on type
    FILE *f = romfs_3ds_fopen(path, "rb");
    if (!f) {
        printf("Failed to open asset: %s\n", path);
        g_asset_cache.count--;
        return NULL;
    }
    
    // Get size
    romfs_3ds_fseek(f, 0, SEEK_END);
    asset->size = (u32)romfs_3ds_ftell(f);
    romfs_3ds_fseek(f, 0, SEEK_SET);
    
    // Allocate and read
    asset->data = linearAlloc(asset->size);
    if (!asset->data) {
        printf("Failed to allocate memory for asset: %s\n", path);
        romfs_3ds_fclose(f);
        g_asset_cache.count--;
        return NULL;
    }
    
    romfs_3ds_fread(asset->data, 1, asset->size, f);
    romfs_3ds_fclose(f);
    
    asset->loaded = 1;
    printf("Loaded asset: %s (%d bytes)\n", path, asset->size);
    
    return asset;
}

void asset_loader_unload(Asset3DS *asset) {
    if (!asset || !asset->loaded) return;
    
    if (asset->data) {
        linearFree(asset->data);
        asset->data = NULL;
    }
    
    asset->loaded = 0;
    asset->size = 0;
}

void asset_loader_unload_all(void) {
    for (u32 i = 0; i < g_asset_cache.count; i++) {
        asset_loader_unload(&g_asset_cache.assets[i]);
    }
    
    g_asset_cache.count = 0;
}

TextureData3DS* asset_loader_load_texture(const char *path) {
    Asset3DS *asset = asset_loader_load(path, ASSET_TYPE_TEXTURE);
    if (!asset) return NULL;
    
    // Parse texture data
    // In real implementation, this would parse N64 texture format
    TextureData3DS *tex = (TextureData3DS *)linearAlloc(sizeof(TextureData3DS));
    if (!tex) return NULL;
    
    tex->data = asset->data;
    tex->size = asset->size;
    tex->width = 32;  // Would be parsed from header
    tex->height = 32;
    tex->format = 0; // RGBA16
    
    return tex;
}

ModelData3DS* asset_loader_load_model(const char *path) {
    Asset3DS *asset = asset_loader_load(path, ASSET_TYPE_MODEL);
    if (!asset) return NULL;
    
    // Parse model data
    ModelData3DS *model = (ModelData3DS *)linearAlloc(sizeof(ModelData3DS));
    if (!model) return NULL;
    
    // In real implementation, this would parse N64 display list
    model->vtxCount = 0;
    model->triCount = 0;
    model->vertices = NULL;
    model->indices = NULL;
    
    return model;
}

LevelData3DS* asset_loader_load_level(const char *path) {
    Asset3DS *asset = asset_loader_load(path, ASSET_TYPE_LEVEL);
    if (!asset) return NULL;
    
    LevelData3DS *level = (LevelData3DS *)linearAlloc(sizeof(LevelData3DS));
    if (!level) return NULL;
    
    // Parse level header
    // In real implementation, this would parse actual level format
    level->geoData = asset->data;
    level->geoSize = asset->size;
    level->collisionData = NULL;
    level->collisionSize = 0;
    
    return level;
}

void* asset_loader_load_sound(const char *path, u32 *size) {
    Asset3DS *asset = asset_loader_load(path, ASSET_TYPE_SOUND);
    if (!asset) return NULL;
    
    *size = asset->size;
    return asset->data;
}

void* asset_loader_load_music(const char *path, u32 *size) {
    Asset3DS *asset = asset_loader_load(path, ASSET_TYPE_MUSIC);
    if (!asset) return NULL;
    
    *size = asset->size;
    return asset->data;
}

void asset_loader_register_texture(u32 texId, const char *path) {
    if (texId >= 1024) return;
    
    // Load and cache texture
    g_textures[texId] = asset_loader_load_texture(path);
}

TextureData3DS* asset_loader_get_texture(u32 texId) {
    if (texId >= 1024) return NULL;
    return g_textures[texId];
}

void asset_loader_register_model(u32 modelId, const char *path) {
    if (modelId >= 1024) return;
    
    g_models[modelId] = asset_loader_load_model(path);
}

ModelData3DS* asset_loader_get_model(u32 modelId) {
    if (modelId >= 1024) return NULL;
    return g_models[modelId];
}

u32 asset_loader_file_exists(const char *path) {
    return romfs_3ds_exists(path);
}

u32 asset_loader_file_size(const char *path) {
    return (u32)romfs_3ds_file_size(path);
}

void* asset_loader_file_read(const char *path, u32 *size) {
    FILE *f = romfs_3ds_fopen(path, "rb");
    if (!f) return NULL;
    
    romfs_3ds_fseek(f, 0, SEEK_END);
    *size = (u32)romfs_3ds_ftell(f);
    romfs_3ds_fseek(f, 0, SEEK_SET);
    
    void *data = linearAlloc(*size);
    if (!data) {
        romfs_3ds_fclose(f);
        return NULL;
    }
    
    romfs_3ds_fread(data, 1, *size, f);
    romfs_3ds_fclose(f);
    
    return data;
}

void asset_loader_load_segment(u32 segment, const char *path) {
    // Load a segment into the N64 memory space
    // This provides compatibility with N64 segment-based addressing
    u32 size;
    void *data = asset_loader_file_read(path, &size);
    
    if (data) {
        // In real implementation, this would set up the segment table
        printf("Loaded segment %d from %s (%d bytes)\n", segment, path, size);
    }
}

void asset_loader_set_cache_size(u32 maxAssets) {
    if (g_asset_cache.assets) {
        free(g_asset_cache.assets);
    }
    
    g_asset_cache.capacity = maxAssets;
    g_asset_cache.assets = (Asset3DS *)malloc(sizeof(Asset3DS) * maxAssets);
    g_asset_cache.count = 0;
}

void asset_loader_clear_cache(void) {
    asset_loader_unload_all();
}

u32 asset_loader_get_cache_usage(void) {
    return g_asset_cache.count;
}

void asset_loader_set_texture_dir(const char *dir) {
    strncpy(g_texture_dir, dir, sizeof(g_texture_dir) - 1);
}

void asset_loader_set_model_dir(const char *dir) {
    strncpy(g_model_dir, dir, sizeof(g_model_dir) - 1);
}

void asset_loader_set_level_dir(const char *dir) {
    strncpy(g_level_dir, dir, sizeof(g_level_dir) - 1);
}

void asset_loader_set_sound_dir(const char *dir) {
    strncpy(g_sound_dir, dir, sizeof(g_sound_dir) - 1);
}

AssetCache3DS* asset_loader_get_cache(void) {
    return &g_asset_cache;
}

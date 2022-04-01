#ifndef GFX_PC_H
#define GFX_PC_H

#include <stdbool.h>
#include <stdint.h>
#include <unordered_map>

struct GfxRenderingAPI;
struct GfxWindowManagerAPI;

struct GfxDimensions
{
    uint32_t internal_mul;
    uint32_t width, height;
    float aspect_ratio;
};

struct TextureCacheKey {
    const uint8_t* texture_addr;
    const uint8_t* palette_addr;
    uint8_t fmt, siz;
    uint8_t palette_index;

    bool operator==(const TextureCacheKey&) const noexcept = default;
     
    struct Hasher {
        size_t operator()(const TextureCacheKey& key) const noexcept {
            uintptr_t addr = (uintptr_t)key.texture_addr;
            return (size_t)(addr ^ (addr >> 5));
        }
    };
};

typedef std::unordered_map<TextureCacheKey, struct TextureCacheValue, TextureCacheKey::Hasher> TextureCacheMap;
typedef std::pair<const TextureCacheKey, struct TextureCacheValue> TextureCacheNode;

struct TextureCacheValue {
    uint32_t texture_id;
    uint8_t cms, cmt;
    bool linear_filter;

    // Old versions of libstdc++ fail to compile this
#ifdef _MSC_VER
    std::list<TextureCacheMap::iterator>::iterator lru_location;
#else
    std::list<int>::iterator lru_location;
#endif
};

#ifdef __cplusplus
extern "C" {
#endif
extern struct GfxDimensions gfx_current_dimensions;

void gfx_init(struct GfxWindowManagerAPI* wapi, struct GfxRenderingAPI* rapi, const char* game_name, bool start_in_fullscreen);
struct GfxRenderingAPI* gfx_get_current_rendering_api(void);
void gfx_start_frame(void);
void gfx_run(Gfx* commands);
void gfx_end_frame(void);
void gfx_set_framedivisor(int);
void gfx_texture_cache_clear();
int gfx_create_framebuffer(uint32_t width, uint32_t height);
uint16_t gfx_get_pixel_depth(float x, float y);

#ifdef __cplusplus
}
#endif
#endif

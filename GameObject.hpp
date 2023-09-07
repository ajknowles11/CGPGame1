#pragma once

#include "SpriteAtlas.hpp"
#include <vector>

class GameObject {
public:
    GameObject();
    ~GameObject();
    virtual void update(float elapsed);
    uint8_t get_max_sprites();
    uint8_t get_sprite_spec();
    std::vector<uint8_t> sprite_table_indices;

    glm::vec2 at = glm::vec2(128, 32);
    bool render_behind = false;
protected:
    uint8_t max_sprite_count;
    uint8_t currentSpriteSpec;
    
};
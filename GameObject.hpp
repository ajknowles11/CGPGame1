#pragma once

#include "SpriteAtlas.hpp"
#include <vector>

class GameObject {
public:
    GameObject();
    ~GameObject();
    virtual void update(float elapsed);
    virtual uint8_t get_max_sprites();
    virtual uint8_t get_sprite_spec();
    std::vector<uint8_t> sprite_table_indices;

    virtual void dmg();

    glm::vec2 at = glm::vec2(128, 32);
    bool render_behind = false;

    bool deleted;
protected:
    uint8_t max_sprite_count;
    uint8_t currentSpriteSpec;
    
};
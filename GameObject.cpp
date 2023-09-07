#include "GameObject.hpp"

GameObject::GameObject() {
}

GameObject::~GameObject(){
}

void GameObject::update(float elapsed) {
}

uint8_t GameObject::get_max_sprites() {
    return max_sprite_count;
}

uint8_t GameObject::get_sprite_spec() {
    return currentSpriteSpec;
} 

void GameObject::dmg() {
}
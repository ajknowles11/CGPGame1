#include "Player.hpp"

Player::Player() {

}

Player::~Player() {
}

void Player::walk_input(bool left, bool right, float elapsed) {
    if (left == right) {
        //decelerate
        float to_add = walk_decel * elapsed;
        if (velocity.x < 0) {
            velocity.x = glm::clamp(velocity.x + to_add, velocity.x, 0.0f);
        }
        else {
            velocity.x = glm::clamp(velocity.x - to_add, 0.0f, velocity.x);
        }
        return;
    }
    float to_add = left ? -walk_accel * elapsed : walk_accel * elapsed;
    velocity.x = glm::clamp(velocity.x + to_add, -max_walk_speed, max_walk_speed);
}

#include "Player.hpp"

Player::Player() {

}

Player::~Player() {
}

void Player::update(float elapsed) {
    update_walk_velocity(walk_dir, elapsed);
    update_fall_velocity(elapsed);
}

void Player::update_walk_velocity(int8_t dir, float elapsed) {
    // if not moving in direction, character decelerates
    if (dir == 0) {
        float to_add = walk_decel * elapsed;
        if (velocity.x < 0) {
            velocity.x = glm::clamp(velocity.x + to_add, velocity.x, 0.0f);
        }
        else {
            velocity.x = glm::clamp(velocity.x - to_add, 0.0f, velocity.x);
        }
        return;
    }

    // otherwise we accelerate in movement dir until max speed
    float to_add = dir < 0 ? -walk_accel * elapsed : walk_accel * elapsed;
    velocity.x = glm::clamp(velocity.x + to_add, -max_walk_speed, max_walk_speed);
}

void Player::update_fall_velocity(float elapsed) {
    if (at.y <= 0) {
		at.y = 0;
        if (velocity.y < 0) velocity.y = 0;
	} 
	else {
		velocity.y -= gravity * elapsed;
	}

	at += velocity * elapsed;
}

void Player::jump() {
    velocity.y += 200.0;
}

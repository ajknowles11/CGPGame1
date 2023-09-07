#include "Enemy.hpp"

Enemy::Enemy(std::shared_ptr<Player> pl) {
    currentSpriteSpec = 0;
    player = pl;
    atk_time = 2*atk_length;
}

Enemy::~Enemy() {
}

void Enemy::update(float elapsed) {
    if (player) {
        if (glm::abs(player->at.x - at.x) < 8) {
            walk_dir = 0;
            if (!is_attacking) {
                if (atk_time >= 2*atk_length) {
                    attack();
                    atk_time = 0;
                }
            }
        }
        else {
            walk_dir = player->at.x > at.x ? 1 : -1;
        }
        update_walk_velocity(walk_dir, elapsed);
    }
    update_fall_velocity(elapsed);

    if (is_on_ground && velocity.x != 0) {
        walk_anim_time += elapsed;
        while (walk_anim_time >= walk_anim_length) {
            walk_anim_time -= walk_anim_length;
        }
    }
    else {
        walk_anim_time = 0;
    }

    atk_time += elapsed;
    if (is_attacking) {
        if (atk_time >= atk_length) {
            is_attacking = false;
        }
    }

    update_sprite_spec();
}

uint8_t Enemy::get_max_sprites() {
    return max_sprite_count;
}

uint8_t Enemy::get_sprite_spec() {
    return current_sprite_spec;
}

void Enemy::update_walk_velocity(int8_t dir, float elapsed) {
    if (dir < 0 && !facing_left) {
        facing_left = true;
    }
    else if (dir > 0 && facing_left) {
        facing_left = false;
    }
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

void Enemy::update_fall_velocity(float elapsed) {
    if (at.y <= 0) {
		at.y = 0;
        is_on_ground = true;
        if (velocity.y < 0) { // this means we just landed
            velocity.y = 0;
            jump_count = 0;
        }
	} 
	else {
        if (jump_count == 0) { // this means we just started falling (not in air yet)
            jump_count += 1;
        }
        is_on_ground = false;
		velocity.y -= gravity * elapsed;
	}

	at += velocity * elapsed;
}

void Enemy::jump() {
    if (jump_count >= max_num_jumps) {
        return;
    }
    jump_count += 1;
    velocity.y = 200.0;
    is_on_ground = false;
}

void Enemy::attack() {
    is_attacking = true;
}

void Enemy::update_sprite_spec() {
    if (is_on_ground) {
        if (!facing_left) {
            if (walk_anim_time < 0.1f) {
                current_sprite_spec = is_attacking ? 6 : 0;
            }
            else if (walk_anim_time < 0.2f) {
                current_sprite_spec = is_attacking ? 7 : 1;
            }
            else {
                current_sprite_spec = is_attacking ? 8 : 2;
            }
        }
        else {
            if (walk_anim_time < 0.1f) {
                current_sprite_spec = is_attacking ? 9 : 3;
            }
            else if (walk_anim_time < 0.2f) {
                current_sprite_spec = is_attacking ? 10 : 4;
            }
            else {
                current_sprite_spec = is_attacking ? 11 : 5;
            }
        }
    }
    else {
        if (!facing_left) {
            current_sprite_spec = is_attacking ? 6 : 0;
        }
        else {
            current_sprite_spec = is_attacking ? 9 : 3;
        }
    }
}

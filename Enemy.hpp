#pragma once

#include "GameObject.hpp"
#include <glm/glm.hpp>
#include "SpriteAtlas.hpp"
#include "Player.hpp"

class Enemy : public GameObject {
public:
	Enemy(std::shared_ptr<Player> pl);
	virtual ~Enemy();

	virtual void update(float elapsed) override;

	void jump();

	void attack();

	bool facing_left = 0;
	int8_t walk_dir = 0; // this is just -1, 0, or 1
	glm::vec2 velocity = glm::vec2(0.0f);


	virtual uint8_t get_max_sprites() override;
    virtual uint8_t get_sprite_spec() override;

    std::shared_ptr<Player> player;

protected:

	uint8_t hp = 1;

	float walk_accel = 1000.0f;
	float walk_decel = 800.0f;
	float max_walk_speed = 60.0f;

	float gravity = 500.0f;
	float max_fall_speed = 200.0f;

	bool is_on_ground;

	uint8_t max_num_jumps = 1;
	uint8_t jump_count = 0;

	uint8_t current_sprite_spec = 0;
	uint8_t max_sprite_count = 4;

	const float walk_anim_length = 0.3f;
	float walk_anim_time = 0;

	const float atk_length = 0.5f;
	float atk_time = 0;
	bool is_attacking;

	void update_walk_velocity(int8_t dir, float elapsed);
	void update_fall_velocity(float elapsed);

	void update_sprite_spec();
};

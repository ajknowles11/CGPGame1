#include <glm/glm.hpp>

class Player {
public:
	Player();
	virtual ~Player();

	void update(float elapsed);

	void jump();

	int8_t walk_dir = 0; // this is just -1, 0, or 1
	glm::vec2 at = glm::vec2(30, 30);
	glm::vec2 velocity = glm::vec2(0.0f);

protected:
	float walk_accel = 1000.0f;
	float walk_decel = 800.0f;
	float max_walk_speed = 100.0f;

	float gravity = 500.0f;
	float max_fall_speed = 200.0f;

	void update_walk_velocity(int8_t dir, float elapsed);
	void update_fall_velocity(float elapsed);
};

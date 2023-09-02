#include <glm/glm.hpp>

class Player {
public:
	Player();
	virtual ~Player();

	void walk_input(bool left, bool right, float elapsed);

	glm::vec2 at = glm::vec2(30, 30);
	glm::vec2 velocity = glm::vec2(0.0f);

	float walk_accel = 1000.0f;
	float walk_decel = 800.0f;

	float max_walk_speed = 100.0f;
	float max_fall_speed = 200.0f;
};

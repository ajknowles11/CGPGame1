#include <glm/glm.hpp>

class Player {
public:
	Player();
	virtual ~Player();

	glm::vec2 at = glm::vec2(30, 30);
};

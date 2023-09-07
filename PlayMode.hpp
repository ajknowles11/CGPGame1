#include "data_path.hpp"
#include "GameObject.hpp"
#include "Load.hpp"
#include "Mode.hpp"
#include "Player.hpp"
#include "PPU466.hpp"
#include "SpriteAtlas.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//sprite resources
	std::vector<SpriteSpec> spec_table;

	//----- game state -----

	bool try_spawn_object(std::shared_ptr<GameObject> obj, glm::vec2 pos = glm::vec2(0,0));

	uint8_t sprite_count = 0;

	std::shared_ptr<Player> player;
	std::vector<std::shared_ptr<GameObject>> game_objects;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
		uint8_t last_pressed = 0;
	} left, right, down, up, attack;

	//some weird background animation:
	float background_fade = 0.0f;

	//----- drawing handled by PPU466 -----
	std::array<bool, 64> sprite_activity;

	PPU466 ppu;
};

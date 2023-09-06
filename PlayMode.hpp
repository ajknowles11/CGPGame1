#include "data_path.hpp"
#include "Load.hpp"
#include "Mode.hpp"
#include "Player.hpp"
#include "PPU466.hpp"
#include "SpriteAtlas.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

Load<SpriteAtlas> sprite_atlas(LoadTagDefault, [&](){
	return new SpriteAtlas(data_path("assets/dojo.atlas"));
});

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	Player player;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
		uint8_t last_pressed = 0;
	} left, right, down, up;

	//some weird background animation:
	float background_fade = 0.0f;

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};

#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

Load<SpriteAtlas> sprite_atlas(LoadTagEarly, [&](){
	return new SpriteAtlas(data_path("game1.atlas"));
});

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	// fill in ppu fields with sprite atlas data
	ppu.palette_table = sprite_atlas->palette_table;
	ppu.tile_table = sprite_atlas->tile_table;
	spec_table = sprite_atlas->spec_table;

	for (uint8_t i = 0; i < 64; i++) {
		ppu.sprites[i].x = 0;
		ppu.sprites[i].y = 240;
	}
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_z) {
			attack.downs += 1;
			attack.pressed = true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_z) {
			attack.pressed = false;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	static bool game_over = false;
	if (game_over) {
		return;
	}
	static bool player_spawned = false;
	if (!player_spawned && sprite_atlas) {
		// spawn player
		player = std::make_shared<Player>();
		player_spawned = try_spawn_object(player, glm::vec2(128,0));
		try_spawn_object(std::make_shared<Enemy>(player), glm::vec2(0,0));
	}

	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);

	// pass walk input to player
	if (left.pressed && !right.pressed) player->walk_dir = -1;
	else if (right.pressed && !left.pressed) player->walk_dir = 1;
	else player->walk_dir = 0;

	// pass jump input to player, only when up pressed (not repeatedly)
	if (up.pressed && !up.last_pressed) {
		player->jump();
	}

	// pass attack input to player
	if (attack.pressed && !attack.last_pressed) {
		player->attack(game_objects);
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	attack.downs = 0;

	static std::vector<std::shared_ptr<GameObject>> deleted;

	// update GameObjects
	for (auto obj : game_objects) {
		obj->update(elapsed);
		if (obj->deleted) {
			deleted.emplace_back(obj);
		}
	}

	// delete any
	for (auto obj : deleted) {
		if (obj == player) {
			game_over = true;
		}
		delete_object(obj);
	}

	// update last_pressed value of inputs (must be after used)
	left.last_pressed = left.pressed;
	right.last_pressed = right.pressed;
	up.last_pressed = up.pressed;
	down.last_pressed = down.pressed;
	attack.last_pressed = attack.pressed;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//handle game object sprites
	for (auto obj : game_objects) {
		SpriteSpec spec = spec_table[obj->get_sprite_spec()];
		std::vector<uint8_t> sprite_idxs = obj->sprite_table_indices;

		for (int i = 0; i < spec.tiles.size(); i++) {
			ppu.sprites[sprite_idxs[i]].x = int8_t(obj->at.x) + (spec.tiles[i].offset.x - 3)*8;
			ppu.sprites[sprite_idxs[i]].y = int8_t(obj->at.y) + (spec.tiles[i].offset.y - 3)*8;
			ppu.sprites[sprite_idxs[i]].index = spec.tiles[i].tile_index;
			ppu.sprites[sprite_idxs[i]].attributes = spec.tiles[i].palette_index;
			if (obj->render_behind) ppu.sprites[sprite_idxs[i]].attributes |= 0x80;
		}
	}

	ppu.background_color = glm::u8vec4(0xD2, 0x66, 0x00, 0xFF);

	//--- actually draw ---
	ppu.draw(drawable_size);
}

bool PlayMode::try_spawn_object(std::shared_ptr<GameObject> obj, glm::vec2 pos) {
	// don't spawn anything if it would exceed the sprite limit
	if (sprite_count + obj->get_max_sprites() > 64) {
		return false;
	}
	// allocate sprite table indices for object
	uint8_t allocated_count = 0;
	std::vector<uint8_t> idxs;
	for (uint8_t i = 0; i < 64; i++) {
		if (allocated_count == obj->get_max_sprites()) break;
		if (!sprite_activity[i]) {
			sprite_activity[i] = true;
			idxs.emplace_back(i);
			allocated_count += 1;
			sprite_count += 1;
		}
	}
	obj->sprite_table_indices = idxs;

	obj->at = pos;
	game_objects.emplace_back(obj);
	return true;
}

void PlayMode::delete_object(std::shared_ptr<GameObject> obj) {
	SpriteSpec spec = spec_table[obj->get_sprite_spec()];
	std::vector<uint8_t> sprite_idxs = obj->sprite_table_indices;

	for (uint8_t i = 0; i < game_objects.size(); i++) {
		if (game_objects[i] == obj) {
			game_objects.erase(game_objects.begin() + i);
			break;
		}
	}

	// remove allocated sprites
	for (auto i : sprite_idxs) {
		sprite_activity[i] = true;
		ppu.sprites[i].y = 240;
		sprite_count -= 1;
	}
}

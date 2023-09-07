#include "PPU466.hpp"
#include "read_write_chunk.hpp"
#include <glm/glm.hpp>
#include <array>
#include <string>
#include <fstream>

// Struct for sprite specifiers, which define the primitive sprites needed to draw a game object including palettes and offsets.
// Base structure inspired by Jim McCann's BigSprite declaration in Discord chat
struct SpriteSpec {
    SpriteSpec(std::vector<uint8_t> plt_ids, std::vector<uint8_t> tile_ids, std::vector<glm::i8vec2> offsets);
    ~SpriteSpec();
	struct Tile {
        uint8_t palette_index; //index into palette pool shared by all SpriteSpecs 
        uint8_t tile_index; //index into tile pool shared by all SpriteSpecs 
        glm::i8vec2 offset; //offset of tile relative to character base point
    };
  std::vector<Tile> tiles;
};

struct SpriteAtlas {
    SpriteAtlas(std::string atlas_path);
    ~SpriteAtlas();
    std::array<PPU466::Palette, 8> palette_table;
    std::array<PPU466::Tile, 16 * 16> tile_table;
    std::vector<SpriteSpec> specs;
};
#include "PPU466.hpp"
#include "read_write_chunk.hpp"
#include <glm/glm.hpp>
#include <array>
#include <string>
#include <fstream>

struct SpriteAtlas {
    SpriteAtlas(std::string atlas_path);
    ~SpriteAtlas();
    std::array< PPU466::Tile, 16 * 16 > tile_table;
};
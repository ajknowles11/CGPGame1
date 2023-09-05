#include "SpriteAtlas.hpp"

SpriteAtlas::SpriteAtlas(std::string atlas_path) {
    std::ifstream atlas(atlas_path, std::ios::binary);
    std::vector< PPU466::Tile > tile_vec;
    read_chunk(atlas, "chr0", &tile_vec);
    for (uint8_t i = 0; i < tile_vec.size(); i++) {
        tile_table[i] = tile_vec[i];
    }
}

SpriteAtlas::~SpriteAtlas() {
}

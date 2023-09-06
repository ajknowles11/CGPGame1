#include "SpriteAtlas.hpp"

SpriteSpec::SpriteSpec(std::vector<uint8_t> tile_ids, std::vector<uint8_t> plt_ids, std::vector<glm::i8vec2> offsets) {
    assert(tile_ids.size() == plt_ids.size() && plt_ids.size() == offsets.size());
    for (uint8_t i = 0; i < offsets.size(); i++) {
        tiles.emplace_back(Tile{tile_ids[i], plt_ids[i], offsets[i]});
    }
}  

SpriteSpec::~SpriteSpec() {
}

SpriteAtlas::SpriteAtlas(std::string atlas_path) {
    std::ifstream atlas(atlas_path, std::ios::binary);
    std::vector<PPU466::Palette> palette_vec;
    std::vector<PPU466::Tile> tile_vec;
    read_chunk(atlas, "plt0", &palette_vec);
    read_chunk(atlas, "til1", &tile_vec);
    read_chunk(atlas, "spc2", &specs);

    assert(palette_vec.size() <= 8);
    assert(tile_vec.size() <= 16 * 16);

    for (uint8_t i = 0; i < palette_vec.size(); i++) {
        palette_table[i] = palette_vec[i];
    }
    for (uint8_t i = 0; i < tile_vec.size(); i++) {
        tile_table[i] = tile_vec[i];
    }
}

SpriteAtlas::~SpriteAtlas() {
}

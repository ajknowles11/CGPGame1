#include "SpriteAtlas.hpp"

SpriteSpec::SpriteSpec(std::vector<SpecTile> spec_tiles, SpecInfo const sinfo) {
    tiles = std::vector<SpecTile>(spec_tiles.begin() + sinfo.spec_start, spec_tiles.begin() + sinfo.spec_end);
}  

SpriteSpec::~SpriteSpec() {
}

SpriteAtlas::SpriteAtlas(std::string const &atlas_path) {
    std::ifstream atlas(atlas_path, std::ios::binary);
    std::vector<PPU466::Palette> palette_vec;
    std::vector<PPU466::Tile> tile_vec;
    std::vector<SpecTile> spec_tiles;
    std::vector<SpecInfo> spec_infos;
    read_chunk(atlas, "plt0", &palette_vec);
    read_chunk(atlas, "til1", &tile_vec);
    read_chunk(atlas, "spc2", &spec_tiles);
    read_chunk(atlas, "spi3", &spec_infos);

    for (auto const sinfo : spec_infos) {
        spec_table.emplace_back(SpriteSpec(spec_tiles, sinfo));
    }

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

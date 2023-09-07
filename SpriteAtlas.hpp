#pragma once

#include "PPU466.hpp"
#include "read_write_chunk.hpp"
#include <glm/glm.hpp>
#include <array>
#include <string>
#include <fstream>

// Helper struct for SpriteSpecifiers. Idea from the characterinfo example in assets pt 2
struct SpecInfo {
    uint8_t spec_start, spec_end;
};

struct SpecTile {
    uint8_t palette_index; //index into palette pool shared by all SpriteSpecs 
    uint8_t tile_index; //index into tile pool shared by all SpriteSpecs 
    glm::i8vec2 offset; //offset of tile relative to character base point
};

// Struct for sprite specifiers, which define the primitive sprites needed to draw a game object including palettes and offsets.
// Base structure inspired by Jim McCann's BigSprite declaration in Discord chat
struct SpriteSpec {
    SpriteSpec(std::vector<SpecTile> spec_tiles, SpecInfo const sinfo);
    ~SpriteSpec();
  std::vector<SpecTile> tiles;
};

struct SpriteAtlas {
    SpriteAtlas(std::string const &atlas_path);
    ~SpriteAtlas();
    std::array<PPU466::Palette, 8> palette_table;
    std::array<PPU466::Tile, 16 * 16> tile_table;
    std::vector<SpriteSpec> spec_table;
};
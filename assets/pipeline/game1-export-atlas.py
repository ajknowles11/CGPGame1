#! /usr/bin/env python

from gimpfu import *
import struct

def group_layers(image):
    specs = []
    tiles = []
    palettes = []

    for l in image.layers:
        if l.name[0] == 's' and l.name[1] == '_':
            specs.append(l)
        elif l.name[0] == 't' and l.name[1] == '_':
            tiles.append(l)
        elif l.name[0] == 'p' and l.name[1] == '_':
            palettes.append(l)
    
    return (specs, tiles, palettes)

def read_tiles_in_layer(layer, palette_list, tile_list, specs_string):
    for j in range(layer.height / 8):
            for i in range(layer.width / 8):
                px_rgn = layer.get_pixel_rgn(i*8, j*8, 8, 8)
                # get colors from tile and output to palette string
                this_palette = []
                this_tile = []
                for v in range(8):
                    for u in range(8):
                        px_color = tuple(px_rgn[i*8+u, j*8+7-v])
                        # Make transparent also black
                        if px_color[3] == '\x00':
                            px_color = tuple(['\x00', '\x00', '\x00', '\x00'])
                        if not px_color in this_palette:
                            if len(this_palette) >= 4:
                                raise Exception("Too many colors in sprite: ({},{}) at pixel ({},{}). Colors: {}".format(i, j, u, v, this_palette))
                            # Add color to palette
                            this_palette.append(px_color)
                        # Collect color indices for tile
                        this_tile.append(this_palette.index(px_color))
                # Discard if empty palette
                if len(this_palette) == 1 and this_palette[0] == tuple(['\x00', '\x00', '\x00', '\x00']):
                    continue
                # Search palettes to see if this palette exists
                p_index = -1
                for p in palette_list:
                    # Check if palette is subset of existing palette
                    if all(clr in p for clr in this_palette):
                        p_index = palette_list.index(p)
                        break
                    # Could also explore replacing existing palettes if wasteful ones were generated, but I don't think it matters.

                if p_index == -1:
                    if len(palette_list) >= 8:
                        raise Exception("No valid palette found for sprite: ({},{}). Colors: {}. Palette: {}".format(i,j, this_palette, palette_list))
                    # If this is a new palette (and we don't already have 8), add it to the list
                    while len(this_palette) < 4:
                        this_palette.append(tuple(['\x00', '\x00', '\x00', '\x00']))
                    p_index = len(palette_list)
                    palette_list.append(this_palette)
                else:
                    # If the palette already exists in the list, we need to swap indices as they may be out of order
                    this_tile = map(lambda i: palette_list[p_index].index(this_palette[i]), this_tile)
                # Now search for tile in tile list
                t_index = -1
                this_tile_tuple = tuple(this_tile)
                for t in tile_list:
                    if tuple(t) == this_tile_tuple:
                        t_index = tile_list.index(t)
                        break
                if t_index == -1:
                    if len(tile_list) >= 256:
                        raise Exception("Tile not found in full tile atlas!")
                    # If this is a new tile and we aren't at capacity, add it to list
                    t_index = len(tile_list)
                    tile_list.append(this_tile)
                # Now we add information to output strings
                # Assign palette index to tile
                specs_string += chr(p_index)
                # Assign tile atlas index to tile
                specs_string += chr(t_index)
                # Assign offset for tile (assumes 7 by 7 tile layout for sprite specs!!!)
                specs_string += chr(i) + chr(6-j)
    
    return (palette_list, tile_list, specs_string)

def read_sprites(spec_layers, palette_list):
    tile_list = []
    specs_string = ""
    specinfo_string = ""

    index = 0
    layer_tile_count = 0
    for layer in spec_layers:
        (palette_list, tile_list, specs_string) = read_tiles_in_layer(layer, palette_list, tile_list, specs_string)
        # Use num tiles to set start and end indices for spec info struct
        layer_tile_count = len(tile_list) - layer_tile_count
        specinfo_string += chr(index) + chr(index + layer_tile_count)
        index += layer_tile_count

    return (palette_list, tile_list, specs_string, specinfo_string)

def read_tiles(tile_layers, palette_list, tile_list):
    for layer in tile_layers:
        (palette_list, tile_list, _) = read_tiles_in_layer(layer, palette_list, tile_list, "")
    
    # Convert to PPU tile format
    bit0list = []
    bit1list = []
    for tile in tile_list:
        bit0 = [0 for _ in range(8)]
        bit1 = [0 for _ in range(8)]
        for j in range(8):
            for i in range(8):
                bit0[j] |= (tile[j*8+i] & 0x1) << i
                bit1[j] |= (tile[j*8+i] & 0x2) >> 1 << i
        bit0list.append(bit0)
        bit1list.append(bit1)

    # Now convert bit lists to string
    tiles_string = ""
    for i in range(len(bit0list)):
        for j in range(8):
            tiles_string += struct.pack('<B', bit0list[i][j])
        for j in range(8):
            tiles_string += struct.pack('<B', bit1list[i][j])
    
    return (palette_list, tiles_string)

def read_palettes(palette_layers):
    palette_list = []
    if len(palette_list) < 8:
        for layer in palette_layers:
            p_rgn = layer.get_pixel_rgn(0, 0, 4, 8)
            for j in range(8-len(palette_list)):
                this_palette = []
                for i in range(4):
                    color = tuple(p_rgn[i, j])
                    if color[3] == '\x00': # make sure transparent has 0 rgb (for my sanity later)
                        color = tuple(['\x00', '\x00', '\x00', '\x00'])
                    if not color in this_palette:
                        this_palette.append(color)
                already_exists = False
                if len(this_palette) == 1 and this_palette[0] == tuple(['\x00', '\x00', '\x00', '\x00']):
                    already_exists = True
                for p in palette_list:
                    if all(clr in p for clr in this_palette):
                        already_exists = True
                        break
                    if len(set(p).union(set(this_palette))) <= 4:
                        palette_list[palette_list.index(p)] = this_palette
                        already_exists = True
                if not already_exists:
                    while len(this_palette) < 4:
                        this_palette.append(tuple(['\x00', '\x00', '\x00', '\x00']))
                    palette_list.append(this_palette)
    
    return palette_list

def write_string(filename, output_string):
    file = open(filename, 'w')
    file.write(output_string)
    file.close()

def export_atlas(image, drawable, atlas_filename):
    # Group layers by sprite spec, tiles, or palettes.
    (spec_layers, tile_layers, palette_layers) = group_layers(image)

    # Loop through manually-defined palettes.
    palette_list = read_palettes(palette_layers)

    # Loop through sprite specs, reading data and auto-adding palettes and tiles.
    (palette_list, tile_list, specs_string, specinfo_string) = read_sprites(spec_layers, palette_list)

    # Loop through additional tiles, adding any extra palettes.
    (palette_list, tiles_string) = read_tiles(tile_layers, palette_list, tile_list)

    # Now export data
    palettes_string = ""
    for p in palette_list:
        for clr in p:
            for val in clr:
                palettes_string += val

    write_string(atlas_filename, "plt0" + struct.pack('<I', len(palettes_string)) + palettes_string
                + "til1" + struct.pack('<I', len(tiles_string)) + tiles_string 
                + "spc2" + struct.pack('<I', len(specs_string)) + specs_string 
                + "spi3" + struct.pack('<I', len(specinfo_string)) + specinfo_string)

register(
    "export_atlas",
    "Export palettes, tiles, and sprite specifiers from this image.",
    "Export palettes, tiles, and sprite specifiers from this image.",
    "Andrew Knowles",
    "Andrew Knowles",
    "2023",
    "<Image>/File/ComputerGameProgramming/Export Sprite Atlas",
    "*",
    [
        (PF_FILENAME, "atlas_filename", "Export file path for packed sprite atlas", ".atlas")
    ],
    [],
    export_atlas)

main()
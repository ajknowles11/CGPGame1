#! /usr/bin/env python

from gimpfu import *
import struct

def get_palettes_string(drawable):
    palettes_region = drawable.get_pixel_rgn(0, 0, 4, 8)

    out = ""
    for j in range(8):
        for i in range(4):
            if palettes_region[i,j][3] == '\x00': # make sure transparent has 0 rgb (for my sanity later)
                out += '\x00\x00\x00\x00'
            else:
                out += palettes_region[i,j]

    return out

def get_sprites_string(drawable, palettes_string):
    # first convert palette string to array of colors
    palettes = [[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]] # bad at python so there is probably a better way to do this (that *doesn't* cause ref-related issues)
    for pal in range(8):
        for clr in range(4):
            palettes[pal][clr] = (tuple(palettes_string[16*pal+4*clr+i] for i in range(4)))
    
    # now iterate through sprites
    atlas_out = ""
    # i = horizontal sprite pos, j = vert sprite position.
    # u = horizontal pixel pos (within sprite), v = vert pixel pos.
    # top left origin.
    for j in range((drawable.height / 8)):
        for i in range((drawable.width / 8)):
            if i == 0 and j == 0:
                continue
            px_rgn = drawable.get_pixel_rgn(i*8,j*8,8,8)
            # get colors from tile and figure out what palette it uses
            this_palette = []
            for u in range(8):
                for v in range(8):
                    color = tuple(px_rgn[i*8+u,j*8+v])
                    if color[3] == '\x00':
                        color = tuple(['\x00', '\x00', '\x00', '\x00'])
                    if not color in this_palette:
                        if len(this_palette) >= 4:
                            raise Exception("Too many colors in sprite: ({},{}) at pixel ({},{}). Colors: {}".format(i, j, u, v, this_palette))
                        else:
                            this_palette.append(color)
            # if all transparent just stop reading tiles at this point
            if len(this_palette) <= 1 and this_palette[0][3] == '\x00':
                return atlas_out
            p_index = -1
            for p in palettes:
                if all(clr in p for clr in this_palette):
                    p_index = palettes.index(p)
                    break
            if p_index == -1:
                raise Exception("No valid palette found for sprite: ({},{}). Colors: {}. Palette: {}".format(i,j, this_palette, palettes))

            # now assign tile table indices to sprite and output in atlas string
            for v in range(8):
                for u in range(8):
                    # we subtract col index from 7 because ppu tiles are bottom-to-top
                    color = tuple(px_rgn[i*8+u,j*8+7-v])
                    if (color[3] == '\x00'):
                        color = tuple(['\x00', '\x00', '\x00', '\x00'])
                    atlas_out += chr(palettes[p_index].index(color))
    
    return atlas_out



def group_layers(image):
    specs, tiles, palettes = []

    for l in image.layers:
        match(l.name[0]):
            case 's':
                specs.add(l)
            case 't':
                tiles.add(l)
            case('p'):
                palettes.add(l)
    
    return (specs, tiles, palettes)

def read_sprites(spec_layers):
    specs_string = ""

    # Initialize palette and tile lists
    palette_list = []
    tile_list = []

    # Sprite layers are 56 by 56 with the character origin at the center.
    for layer in spec_layers:
        for j in range(7):
            for i in range(7):
                px_rgn = layer.get_pixel_rgn(i*8, j*8, 8, 8)
                # get colors from tile and output to palette string
                this_palette = []
                this_tile = []
                for v in range(8):
                    for u in range(8):
                        px_color = tuple(px_rgn[i*8+u, j*8+v])
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
                # Search palettes to see if this palette exists
                p_index = -1
                for p in palette_list:
                    if all(clr in p for clr in this_palette):
                        p_index = palette_list.index(p)
                        break
                if p_index == -1:
                    if len(palette_list >= 8):
                        raise Exception("No valid palette found for sprite: ({},{}). Colors: {}. Palette: {}".format(i,j, this_palette, palette_list))
                    # If this is a new palette (and we don't already have 8), add it to the list
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
                    if len(tile_list >= 256):
                        raise Exception("Invalid tile")
                    # If this is a new tile and we aren't at capacity, add it to list
                    t_index = len(tile_list)
                    tile_list.append(this_tile)
                # Now we add information to output strings
                # Assign palette index to tile
                specs_string += chr(p_index)
                # Assign tile atlas index to tile
                specs_string += chr(t_index)
                # Assign offset for tile
                specs_string += chr(i) + chr(6-j)

def read_tiles(tile_layers, palettes_string, tiles_string):

def read_palettes(palette_layers, palettes_string):

def write_string(filename, output_string):
    file = open(filename, 'w')
    file.write(output_string)
    file.close()

def export_atlas(image, drawable, atlas_filename):
    palettes_string = get_palettes_string(drawable)
    atlas_string = get_sprites_string(drawable, palettes_string)

    out_palettes = "chr0" + struct.pack('<I', len(palettes_string)) + palettes_string
    out_atlas = "chr0" + struct.pack('<I', len(atlas_string)) + atlas_string
    write_string(palettes_filename, out_palettes)
    write_string(atlas_filename, out_atlas)



    # Group layers by sprite spec, tiles, or palettes.
    (spec_layers, tile_layers, palette_layers) = group_layers(image)

    # Loop through sprite specs, reading data and auto-adding palettes and tiles.
    (palettes_string, tiles_string, specs_string) = read_sprites(spec_layers)

    # Loop through additional tiles, adding any extra palettes.
    (palettes_string, tiles_string) = read_tiles(tile_layers, palettes_string, tiles_string)

    # Loop through additional palettes.
    (palettes_string) = read_palettes(palette_layers, palettes_string)

    # Now export data
    write_string("plt0" + struct.pack('<I', len(palettes_string)) + palettes_string + "til1" + struct.pack('<I', len(tiles_string)) + tiles_string + "spec2" + struct.pack('<I', len(specs_string)) + specs_string)

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
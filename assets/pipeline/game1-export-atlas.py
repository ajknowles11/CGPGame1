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
    palettes_map_out = ""
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
                return (atlas_out, palettes_map_out)
            p_index = -1
            for p in palettes:
                if all(clr in p for clr in this_palette):
                    p_index = palettes.index(p)
                    break
            if p_index == -1:
                raise Exception("No valid palette found for sprite: ({},{}). Colors: {}. Palette: {}".format(i,j, this_palette, palettes))
            # assign palette in palette map
            palettes_map_out += chr(p_index)

            # now assign tile table indices to sprite and output in atlas string
            for v in range(8):
                for u in range(8):
                    # we subtract col index from 7 because ppu tiles are bottom-to-top
                    color = tuple(px_rgn[i*8+u,j*8+7-v])
                    if (color[3] == '\x00'):
                        color = tuple(['\x00', '\x00', '\x00', '\x00'])
                    atlas_out += chr(palettes[p_index].index(color))
    
    return (atlas_out, palettes_map_out)

def write_string(filename, output_string):
    file = open(filename, 'w')
    file.write(output_string)
    file.close()

def export_atlas(image, drawable, palettes_filename, atlas_filename):
    palettes_string = get_palettes_string(drawable)
    (atlas_string, palettes_map_string) = get_sprites_string(drawable, palettes_string)

    out_palettes = "plt0" + struct.pack('<I', len(palettes_string)) + palettes_string
    out_atlas = "pdx0" + struct.pack('<I', len(palettes_map_string)) + palettes_map_string
    out_atlas += "spr0" + struct.pack('<I', len(atlas_string)) + atlas_string
    write_string(palettes_filename, out_palettes)
    write_string(atlas_filename, out_atlas)

register(
    "export_atlas",
    "Export palettes and sprite atlas from this image.",
    "Export palettes and sprite atlas from this image.",
    "Andrew Knowles",
    "Andrew Knowles",
    "2023",
    "<Image>/File/ComputerGameProgramming/Export Palettes and Atlas",
    "*",
    [
        (PF_FILENAME, "palettes_filename", "Export file path for palettes", ".palettes"),
        (PF_FILENAME, "atlas_filename", "Export file path for sprite atlas (with palette map)", ".atlas")
    ],
    [],
    export_atlas)

main()
from PIL import Image
import os
import argparse
import io
import re
import sys
import platform

# Constants from fxdata-build.py
VERSION = '1.15'
constants = [
    # normal bitmap modes
    ("dbmNormal",    0x00),
    ("dbmOverwrite", 0x00),
    ("dbmWhite",     0x01),
    ("dbmReverse",   0x08),
    ("dbmBlack",     0x0D),
    ("dbmInvert",    0x02),
    # masked bitmap modes for frame
    ("dbmMasked",              0x10),
    ("dbmMasked_dbmWhite",     0x11),
    ("dbmMasked_dbmReverse",   0x18),
    ("dbmMasked_dbmBlack",     0x1D),
    ("dbmMasked_dbmInvert",    0x12),
    # bitmap modes for last bitmap in a frame
    ("dbmNormal_end",    0x40),
    ("dbmOverwrite_end", 0x40),
    ("dbmWhite_end",     0x41),
    ("dbmReverse_end",   0x48),
    ("dbmBlack_end",     0x4D),
    ("dbmInvert_end",    0x42),
    # masked bitmap modes for last bitmap in a frame
    ("dbmMasked_end",              0x50),
    ("dbmMasked_dbmWhite_end",     0x51),
    ("dbmMasked_dbmReverse_end",   0x58),
    ("dbmMasked_dbmBlack_end",     0x5D),
    ("dbmMasked_dbmInvert_end",    0x52),
    # bitmap modes for last bitmap of the last frame
    ("dbmNormal_last",    0x80),
    ("dbmOverwrite_last", 0x80),
    ("dbmWhite_last",     0x81),
    ("dbmReverse_last",   0x88),
    ("dbmBlack_last",     0x8D),
    ("dbmInvert_last",    0x82),
    # masked bitmap modes for last bitmap in a frame
    ("dbmMasked_last",              0x90),
    ("dbmMasked_dbmWhite_last",     0x91),
    ("dbmMasked_dbmReverse_last",   0x98),
    ("dbmMasked_dbmBlack_last",     0x9D),
    ("dbmMasked_dbmInvert_last",    0x92),
]

# Functions from convert-sprites.py
def get_shade(rgba, shades, shade):
    w = (254 + shades) // shades
    b = (shade + 1) * w
    return 1 if rgba[0] >= b else 0

def get_mask(rgba):
    return 1 if rgba[3] >= 128 else 0

def convert(fname, shades, sw=None, sh=None, num=None):
    if not (shades >= 2 and shades <= 4):
        print('shades argument must be 2, 3, or 4')
        return None

    im = Image.open(fname).convert('RGBA')
    pixels = list(im.getdata())

    masked = False
    for i in pixels:
        if i[3] < 255:
            masked = True
            break

    w = im.width
    h = im.height
    if sw is None:
        sw = w
    if sh is None:
        sh = h
    nw = w // sw
    nh = h // sh
    if num is None:
        num = nw * nh
    sp = (sh + 7) // 8

    if nw * nh <= 0:
        print('%s: Invalid sprite dimensions' % fname)
        return None

    bytes = bytearray([sw, sh])

    for n in range(num):
        bx = (n % nw) * sw
        by = (n // nw) * sh
        for shade in range(shades - 1):
            for p in range(sp):
                for ix in range(sw):
                    x = bx + ix
                    byte = 0
                    mask = 0
                    for iy in range(8):
                        y = p * 8 + iy
                        if y >= sh:
                            break
                        y += by
                        i = y * w + x
                        rgba = pixels[i]
                        byte |= (get_shade(rgba, shades, shade) << iy)
                        mask |= (get_mask(rgba) << iy)
                    bytes += bytearray([byte])
                    if masked:
                        bytes += bytearray([mask])

    return bytes

def convert_header(fname, sym, shades=2, sw=None, sh=None, num=None):
    bytes = convert(fname, shades, sw, sh, num)
    if bytes is None:
        return
    f = io.StringIO()
    f.write('uint8_t %s[] =\n{\n' % sym)
    for n in range(len(bytes)):
        if n % 16 == 0:
            f.write('    ')
        f.write('%3d,' % bytes[n])
        f.write(' ' if n % 16 != 15 else '\n')
    if len(bytes) % 16 != 0:
        f.write('\n')
    f.write('};\n')
    return f.getvalue()

def parse_filename(filename):
    name, rest = filename.split('_')
    dimensions, _ = rest.split('.')
    width, height = dimensions.split('x')
    return name, int(width), int(height)

def sort_strings(strings):
    def key_func(s):
        match = re.match(r'(\D+)(\d+)', s)
        prefix, num = match.groups()
        return (prefix, int(num))

    return sorted(strings, key=key_func)

def convert_sprites(dirpath, shades=2, out_dir="."):
    all_buffers = ""
    pattern = re.compile(r'^.*_\d+x\d+\.png$')
    script_dir = os.path.dirname(os.path.abspath(__file__))
    dir_path = os.path.join(script_dir, dirpath)

    for filename in os.listdir(dir_path):
        filepath = os.path.join(dir_path, filename)
        if os.path.isfile(filepath) and pattern.match(filename):
            name, width, height = parse_filename(filename)
            out = convert_header(filepath, name, shades, width, height)
            if out:
                all_buffers += out

    out_path = os.path.join(out_dir, "Sprites.txt")
    with open(out_path, 'w') as f:
        f.write(all_buffers)
    return out_dir

# Functions from fxdata-build.py
def fx_print(s):
    sys.stdout.write(s + '\n')
    sys.stdout.flush()

def raw_data(filename):
    with open(filename, "rb") as file:
        bytes = bytearray(file.read())
        file.close()
        return bytes

def include_file(filename):
    fx_print("Including file {}".format(filename))
    with open(filename, "r") as file:
        lines = file.readlines()
        file.close()
        return lines

def image_data(filename):
    symbols = []
    header = []
    indent = ''
    
    ## parse filename ## FILENAME_[WxH]_[S].[EXT]"
    spriteWidth = 0
    spriteHeight = 0
    spacing = 0
    elements = os.path.basename(os.path.splitext(filename)[0]).split("_")
    lastElement = len(elements)-1
    #get width and height from filename
    i = lastElement
    while i > 0:
        subElements = list(filter(None,elements[i].split('x')))
        if len(subElements) == 2 and subElements[0].isnumeric() and subElements[1].isnumeric():
            spriteWidth = int(subElements[0])
            spriteHeight = int(subElements[1])
            if i < lastElement and elements[i+1].isnumeric():
                spacing = int(elements[i+1])
            break
        else: 
            i -= 1

    #load image
    img = Image.open(filename).convert("RGBA")
    pixels = list(img.getdata())
    #check for transparency
    transparency = False
    for i in pixels:
        if i[3] < 255:
            transparency = True
            break

    # check for multiple frames/tiles
    if spriteWidth > 0:
        hframes = (img.size[0] - spacing) // (spriteWidth + spacing)
    else:
        spriteWidth = img.size[0] - 2 * spacing
        hframes = 1
    if spriteHeight > 0:
        vframes = (img.size[1] - spacing) // (spriteHeight + spacing)
    else:
        spriteHeight = img.size[1] - 2* spacing
        vframes = 1

    #create byte array for bin file
    size = (spriteHeight+7) // 8 * spriteWidth * hframes * vframes
    if transparency:
        size += size
    bytes = bytearray([spriteWidth >> 8, spriteWidth & 0xFF, spriteHeight >> 8, spriteHeight & 0xFF])
    bytes += bytearray(size)
    i = 4
    b = 0
    m = 0
    fy = spacing
    frames = 0
    for v in range(vframes):
        fx = spacing
        for h in range(hframes):
            for y in range(0, spriteHeight, 8):
                for x in range(0, spriteWidth):
                    for p in range(0, 8):
                        b = b >> 1
                        m = m >> 1
                        if (y + p) < spriteHeight:  # for heights not multiple of 8
                            if pixels[(fy + y + p) * img.size[0] + fx + x][1] > 64:
                                b |= 0x80  # white pixel
                            if pixels[(fy + y + p) * img.size[0] + fx + x][3] > 64:
                                m |= 0x80  # opaque pixel
                            else:
                                b &= 0x7F  # clear white pixel for transparent
                    bytes[i] = b
                    i += 1
                    if transparency:
                        bytes[i] = m
                        i += 1
            frames += 1
            fx += spriteWidth + spacing
        fy += spriteHeight + spacing

    return bytes

def build_fxdata(script_file, out_dir):
    fx_print('FX data build tool version {} by Mr.Blinky May 2021 - Mar.2023\nUsing Python version {}'.format(
        VERSION, platform.python_version()))
    
    filename = os.path.abspath(script_file)
    datafilename = os.path.join(out_dir, os.path.splitext(os.path.basename(script_file))[0] + '-data.bin')
    savefilename = os.path.join(out_dir, os.path.splitext(os.path.basename(script_file))[0] + '-save.bin')
    devfilename = os.path.join(out_dir, os.path.splitext(os.path.basename(script_file))[0] + '.bin')
    headerfilename = os.path.join(out_dir, os.path.splitext(os.path.basename(script_file))[0] + '.h')
    path = os.path.dirname(filename) + os.sep
    
    if not os.path.isfile(filename):
        sys.stderr.write("FX data script file not found.\n")
        sys.exit(-1)

    with open(filename, "r") as file:
        lines = file.readlines()
        file.close()

    fx_print("Building FX data using {}".format(filename))
    
    bytes = bytearray()
    symbols = []
    header = []
    label = ''
    indent = ''
    blkcom = False
    namespace = False
    include = False
    saveStart = -1
    t = 0
    
    lineNr = 0
    while lineNr < len(lines):
        parts = [p for p in re.split("([ ,]|[\\'].*[\\'])", lines[lineNr]) if p.strip() and p != ',']
        for i in range(len(parts)):
            part = parts[i]
            # strip unwanted chars
            if part[:1] == '\t': part = part[1:]
            if part[:1] == '{': part = part[1:]
            if part[-1:] == '\n': part = part[:-1]
            if part[-1:] == ';': part = part[:-1]
            if part[-1:] == '}': part = part[:-1]
            if part[-1:] == ';': part = part[:-1]
            if part[-1:] == '.': part = part[:-1]
            if part[-1:] == ',': part = part[:-1]
            if part[-2:] == '[]': part = part[:-2]
            # handle comments
            if blkcom:
                p = part.find('*/', 2)
                if p >= 0:
                    part = part[p+2:]
                    blkcom = False
            else:
                if part[:2] == '//':
                    break
                elif part[:2] == '/*':
                    p = part.find('*/', 2)
                    if p >= 0: 
                        part = part[p+2:]
                    else: 
                        blkcom = True
                # handle types
                elif part == '=': pass
                elif part == 'const': pass
                elif part == 'PROGMEM': pass
                elif part == 'align': t = 0
                elif part == 'int8_t': t = 1
                elif part == 'uint8_t': t = 1
                elif part == 'int16_t': t = 2
                elif part == 'uint16_t': t = 2
                elif part == 'int24_t': t = 3
                elif part == 'uint24_t': t = 3
                elif part == 'int32_t': t = 4
                elif part == 'uint32_t': t = 4
                elif part == 'image_t': t = 5
                elif part == 'raw_t': t = 6
                elif part == 'String': t = 7
                elif part == 'string': t = 7
                elif part == 'include': include = True
                elif part == 'datasection': pass
                elif part == 'savesection': saveStart = len(bytes)
                # handle namespace
                elif part == 'namespace':
                    namespace = True
                elif namespace:
                    namespace = False
                    header.append("namespace {}\n{{".format(part))
                    indent += '  '
                elif part == 'namespace_end':
                    indent = indent[:-2]
                    header.append('}\n')
                    namespace = False
                # handle strings
                elif (part[:1] == "'") or (part[:1] == '"'):
                    if part[:1] == "'": 
                        part = part[1:part.rfind("'")]
                    else:  
                        part = part[1:part.rfind('"')]
                    # handle include
                    if include:
                        lines[lineNr+1:lineNr+1] = include_file(path + part)
                        include = False
                    elif t == 1: 
                        bytes += part.encode('utf-8').decode('unicode_escape').encode('utf-8')
                    elif t == 5: 
                        bytes += image_data(path + part)
                    elif t == 6: 
                        bytes += raw_data(path + part)
                    elif t == 7: 
                        bytes += part.encode('utf-8').decode('unicode_escape').encode('utf-8') + b'\x00'
                    else:
                        sys.stderr.write('ERROR in line {}: unsupported string for type\n'.format(lineNr))
                        sys.exit(-1)
                # handle values
                elif part[:1].isnumeric() or (part[:1] == '-' and part[1:2].isnumeric()):
                    n = int(part, 0)
                    if t == 4: bytes.append((n >> 24) & 0xFF)
                    if t >= 3: bytes.append((n >> 16) & 0xFF)
                    if t >= 2: bytes.append((n >> 8) & 0xFF)
                    if t >= 1: bytes.append((n >> 0) & 0xFF)
                # handle align
                    if t == 0:
                        align = len(bytes) % n
                        if align: 
                            bytes += b'\xFF' * (n - align)
                # handle labels
                elif part[:1].isalpha():
                    for j in range(len(part)):
                        if part[j] == '=':
                            symbols.append((label, len(bytes)))
                            header.append('{}constexpr uint24_t {} = 0x{:06X};'.format(indent, label, len(bytes)))
                            label = ''
                            part = part[j+1:]
                            parts.insert(i+1, part)
                            break
                        elif part[j].isalnum() or part[j] == '_':
                            label += part[j]
                        else:
                            sys.stderr.write('ERROR in line {}: Bad label: {}\n'.format(lineNr, label))
                            sys.exit(-1)
                    if label and i < len(parts) - 1 and parts[i+1][:1] == '=':
                        symbols.append((label, len(bytes)))
                        header.append('{}constexpr uint24_t {} = 0x{:06X};'.format(indent, label, len(bytes)))
                        label = ''
                    # handle included constants
                    if label:
                        found = False
                        for symbol in constants:
                            if symbol[0] == label:
                                if t == 4: bytes.append((symbol[1] >> 24) & 0xFF)
                                if t >= 3: bytes.append((symbol[1] >> 16) & 0xFF)
                                if t >= 2: bytes.append((symbol[1] >> 8) & 0xFF)
                                if t >= 1: bytes.append((symbol[1] >> 0) & 0xFF)
                                label = ''
                                found = True
                                break
                        if not found:
                            # handle symbol values
                            for symbol in symbols:
                                if symbol[0] == label:
                                    if t == 4: bytes.append((symbol[1] >> 24) & 0xFF)
                                    if t >= 3: bytes.append((symbol[1] >> 16) & 0xFF)
                                    if t >= 2: bytes.append((symbol[1] >> 8) & 0xFF)
                                    if t >= 1: bytes.append((symbol[1] >> 0) & 0xFF)
                                    label = ''
                                    found = True
                                    break
                            if not found:
                                sys.stderr.write('ERROR in line {}: Undefined symbol: {}\n'.format(lineNr, label))
                                sys.exit(-1)
                elif len(part) > 0:
                    sys.stderr.write('ERROR unable to parse {} in element: {}\n'.format(part, str(parts)))
                    sys.exit(-1)
        lineNr += 1

    if saveStart >= 0:
        dataSize = saveStart
        dataPages = (dataSize + 255) // 256
        saveSize = len(bytes) - saveStart
        savePages = (saveSize + 4095) // 4096 * 16
    else:
        dataSize = len(bytes)
        dataPages = (dataSize + 255) // 256
        saveSize = 0
        savePages = 0
    dataPadding = dataPages * 256 - dataSize
    savePadding = savePages * 256 - saveSize

    fx_print("Saving FX data header file {}".format(headerfilename))
    with open(headerfilename, "w") as file:
        file.write('#pragma once\n\n')
        file.write('/**** FX data header generated by fxdata-build.py tool version {} ****/\n\n'.format(VERSION))
        file.write('using uint24_t = __uint24;\n\n')
        file.write('// Initialize FX hardware using  FX::begin(FX_DATA_PAGE); in the setup() function.\n\n')
        file.write('constexpr uint16_t FX_DATA_PAGE  = 0x{:04x};\n'.format(65536 - dataPages - savePages))
        file.write('constexpr uint24_t FX_DATA_BYTES = {};\n\n'.format(dataSize))
        if saveSize > 0: 
            file.write('constexpr uint16_t FX_SAVE_PAGE  = 0x{:04x};\n'.format(65536 - savePages))
            file.write('constexpr uint24_t FX_SAVE_BYTES = {};\n\n'.format(saveSize))
        for line in header:
            file.write(line + '\n')
        file.close()

    fx_print("Saving {} bytes FX data to {}".format(dataSize, datafilename))
    with open(datafilename, "wb") as file:
        file.write(bytes[0:dataSize])
        file.close()
    if saveSize > 0:
        fx_print("Saving {} bytes FX savedata to {}".format(saveSize, savefilename))
        with open(savefilename, "wb") as file:
            file.write(bytes[saveStart:len(bytes)])
            file.close()
    fx_print("Saving FX development data to {}".format(devfilename))
    with open(devfilename, "wb") as file:
        file.write(bytes[0:dataSize])
        if dataPadding > 0: 
            file.write(b'\xFF' * dataPadding)
        if saveSize > 0:
            file.write(bytes[saveStart:len(bytes)])
            if savePadding > 0: 
                file.write(b'\xFF' * savePadding)
        file.close()

def main():
    parser = argparse.ArgumentParser(
        description="Combined sprite converter and FX data builder")
    parser.add_argument(
        "dirpath", help="Path to directory with images")
    parser.add_argument("-s", "--shades", type=int, default=2,
                        help="Number of shades (2, 3, or 4)")
    parser.add_argument("-o", "--out", default=".",
                        help="Output directory for generated files")
    parser.add_argument("-f", "--fxdata", default="fxdata.txt",
                        help="Path to FX data script file")
    args = parser.parse_args()

    # Convert sprites first
    output_dir = convert_sprites(args.dirpath, args.shades, args.out)
    
    # Then build FX data
    fxdata_script = os.path.join(output_dir, args.fxdata)
    if not os.path.exists(fxdata_script):
        print(f"FX data script not found at: {fxdata_script}")
        print("Please create an fxdata.txt file in the output directory")
        return
        
    build_fxdata(fxdata_script, output_dir)
    print("Processing completed successfully!")

if __name__ == "__main__":
    main()
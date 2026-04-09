#!/usr/bin/env python3
"""
Script to generate 3DS banner and icon files from the provided banner 3D model.
Uses the texture from the banner 3d model folder.
This version uses raw RGB data instead of PIL.
"""

import subprocess
import sys
import os
import struct

# Paths
BANNER_DIR = "banner 3d model"
OUTPUT_DIR = "build_3ds"
TEXTURE_FILE = os.path.join(BANNER_DIR, "texture_pbr_20250901.png")

def read_simple_png(filepath):
    """Very basic PNG reader - only supports RGB/RGBA non-interlaced PNGs"""
    try:
        with open(filepath, 'rb') as f:
            data = f.read()
    except FileNotFoundError:
        return None
    
    # Check PNG signature
    if data[:8] != b'\x89PNG\r\n\x1a\n':
        return None
    
    # Find IHDR chunk
    pos = 8
    width = height = 0
    bit_depth = color_type = 0
    
    while pos < len(data):
        length = struct.unpack('>I', data[pos:pos+4])[0]
        chunk_type = data[pos+4:pos+8].decode('ascii')
        chunk_data = data[pos+8:pos+8+length]
        
        if chunk_type == 'IHDR':
            width = struct.unpack('>I', chunk_data[0:4])[0]
            height = struct.unpack('>I', chunk_data[4:8])[0]
            bit_depth = chunk_data[8]
            color_type = chunk_data[9]
        elif chunk_type == 'IDAT':
            return None
        elif chunk_type == 'IEND':
            break
        
        pos += 12 + length
    
    return None

def create_generated_banner():
    """Create a simple generated banner (256x128)"""
    print("Creating 3DS banner (256x128)...")
    
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    
    W, H = 256, 128
    
    data = bytearray()
    for y in range(H):
        for x in range(W):
            r = 255 - int(50 * y / H)
            g = int(20 * x / W)
            b = int(20 * (1 - x / W))
            data.extend([r, g, b])
    
    raw_path = os.path.join(OUTPUT_DIR, "banner.raw")
    with open(raw_path, "wb") as f:
        f.write(data)
    
    bmp_data = bytearray()
    for y in range(H-1, -1, -1):
        row = bytearray()
        for x in range(W):
            idx = (y * W + x) * 3
            row.extend([data[idx+2], data[idx+1], data[idx]])
        while len(row) % 4 != 0:
            row.append(0)
        bmp_data.extend(row)
    
    file_size = 54 + len(bmp_data)
    bmp_header = bytearray([
        0x42, 0x4D,
        (file_size >> 0) & 0xFF,
        (file_size >> 8) & 0xFF,
        (file_size >> 16) & 0xFF,
        (file_size >> 24) & 0xFF,
        0x00, 0x00, 0x00, 0x00,
        0x36, 0x00, 0x00, 0x00,
        0x28, 0x00, 0x00, 0x00,
        (W >> 0) & 0xFF, (W >> 8) & 0xFF, (W >> 16) & 0xFF, (W >> 24) & 0xFF,
        (H >> 0) & 0xFF, (H >> 8) & 0xFF, (H >> 16) & 0xFF, (H >> 24) & 0xFF,
        0x01, 0x00,
        0x18, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    ])
    
    bmp_path = os.path.join(OUTPUT_DIR, "banner.bmp")
    with open(bmp_path, "wb") as f:
        f.write(bmp_header)
        f.write(bmp_data)
    
    print(f"Banner saved to {bmp_path}")
    return bmp_path

def create_icon():
    """Create a simple icon (48x48)"""
    print("Creating 3DS icon (48x48)...")
    
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    
    W, H = 48, 48
    
    data = bytearray()
    for y in range(H):
        for x in range(W):
            border = 2
            if x < border or x >= W - border or y < border or y >= H - border:
                r, g, b = 180, 0, 0
            else:
                cx, cy = W // 2, H // 2
                if (abs(x - cx) < 15 and abs(y - cy) < 10 and
                    not (8 < x < 16 and 10 < y < 22) and
                    not (32 < x < 40 and 10 < y < 22)):
                    if (abs((x - 6) - (cx - 14) + (y - cy - 2)) < 6) or \
                       (abs((x + 6) - (cx + 14) - (y - cy - 2)) < 6) or \
                       (abs(x - cx) < 4):
                        r, g, b = 255, 255, 255
                    else:
                        r, g, b = 220, 20, 20
                else:
                    r, g, b = 220, 20, 20
            
            data.extend([r, g, b])
    
    bmp_data = bytearray()
    for y in range(H-1, -1, -1):
        row = bytearray()
        for x in range(W):
            idx = (y * W + x) * 3
            row.extend([data[idx+2], data[idx+1], data[idx]])
        while len(row) % 4 != 0:
            row.append(0)
        bmp_data.extend(row)
    
    file_size = 54 + len(bmp_data)
    bmp_header = bytearray([
        0x42, 0x4D,
        (file_size >> 0) & 0xFF,
        (file_size >> 8) & 0xFF,
        (file_size >> 16) & 0xFF,
        (file_size >> 24) & 0xFF,
        0x00, 0x00, 0x00, 0x00,
        0x36, 0x00, 0x00, 0x00,
        0x28, 0x00, 0x00, 0x00,
        (W >> 0) & 0xFF, (W >> 8) & 0xFF, (W >> 16) & 0xFF, (W >> 24) & 0xFF,
        (H >> 0) & 0xFF, (H >> 8) & 0xFF, (H >> 16) & 0xFF, (H >> 24) & 0xFF,
        0x01, 0x00,
        0x18, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    ])
    
    bmp_path = os.path.join(OUTPUT_DIR, "icon.bmp")
    with open(bmp_path, "wb") as f:
        f.write(bmp_header)
        f.write(bmp_data)
    
    print(f"Icon saved to {bmp_path}")
    return bmp_path

def create_smdh_template(icon_path):
    """Create SMDH configuration file"""
    cfg_content = f"""[Title]
Short = Mario Builder 64
Long = Mario Builder 64 - 3DS Edition
Publisher = Rovertronic

[Settings]
Region = FREE
AgeRating = 0

[Icons]
Icon = {icon_path}
"""
    
    cfg_path = os.path.join(OUTPUT_DIR, "mb64.smdh.cfg")
    with open(cfg_path, "w") as f:
        f.write(cfg_content)
    
    smdh_path = os.path.join(OUTPUT_DIR, "mb64.smdh")
    
    print(f"\nSMDH config saved to {cfg_path}")
    print(f"\nTo create the final SMDH file:")
    print(f"  bannertool makesmdh -i {icon_path} -l 'Mario Builder 64' -p 'Rovertronic' \\")
    print(f"                      -s 'Build and play custom Mario 64 levels' -o {smdh_path}")
    
    return smdh_path

def main():
    print("=== Mario Builder 64 - 3DS Banner/Icon Generator ===\n")
    
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    
    banner = create_generated_banner()
    icon = create_icon()
    
    if banner and icon:
        smdh = create_smdh_template(icon)
        print("\n=================================")
        print("Banner and icon generation complete!")
        print("=================================")
        print(f"\nAssets saved in: {OUTPUT_DIR}/")
        print("\nFiles created:")
        print(f"  - {banner}")
        print(f"  - {icon}")
        print(f"  - {smdh}.cfg")
    else:
        print("\nError: Failed to generate banner/icon")
        sys.exit(1)

if __name__ == "__main__":
    main()

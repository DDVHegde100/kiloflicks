#!/usr/bin/env python3
"""
Create a simple test BMP image for the steganography application
"""
import struct

def create_test_bmp(filename, width=100, height=100):
    """Create a simple 24-bit BMP image"""
    
    # Calculate padding for 4-byte alignment
    row_size = ((width * 3 + 3) // 4) * 4
    image_size = row_size * height
    file_size = 54 + image_size  # 54 bytes for headers
    
    # BMP File Header (14 bytes)
    file_header = struct.pack('<2sIHHI',
        b'BM',          # Signature
        file_size,      # File size
        0,              # Reserved
        0,              # Reserved  
        54              # Offset to pixel data
    )
    
    # BMP Info Header (40 bytes)
    info_header = struct.pack('<IIIHHIIIIII',
        40,             # Header size
        width,          # Width
        height,         # Height
        1,              # Planes
        24,             # Bits per pixel
        0,              # Compression
        image_size,     # Image size
        2835,           # X pixels per meter
        2835,           # Y pixels per meter
        0,              # Colors used
        0               # Important colors
    )
    
    # Create pixel data (gradient pattern)
    pixel_data = bytearray()
    for y in range(height):
        row = bytearray()
        for x in range(width):
            # Create a gradient pattern (BGR format)
            blue = (x * 255) // width
            green = (y * 255) // height
            red = ((x + y) * 255) // (width + height)
            row.extend([blue, green, red])
        
        # Add padding to align to 4 bytes
        while len(row) % 4 != 0:
            row.append(0)
        pixel_data.extend(row)
    
    # Write the BMP file
    with open(filename, 'wb') as f:
        f.write(file_header)
        f.write(info_header)
        f.write(pixel_data)
    
    print(f"Created test BMP image: {filename} ({width}x{height})")

if __name__ == "__main__":
    # Create test images of different sizes
    create_test_bmp("test_small.bmp", 50, 50)
    create_test_bmp("test_medium.bmp", 100, 100) 
    create_test_bmp("test_large.bmp", 200, 200)
    print("Test BMP images created successfully!")

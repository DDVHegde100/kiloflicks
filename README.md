# Thousand Flicks

They say a picture is worth a 1000 words, well now that is actually true. Thousandflicks is a A C++ command-line tool to encode and decode up to 1000 words into images using mathematical image manipulation (not metadata). 

thousandflicks encode <input_image> <output_image> <message_file>
thousandflicks decode <input_image>
g++ -std=c++17 -o thousandflicks src/main.cpp

## Commands

```
thousandflicks encode <input_bmp> <output_bmp> <message_file>
	# Encode a message from a file into a BMP image

thousandflicks encode-text <input_bmp> <output_bmp> <message_string>
	# Encode a short ASCII/UTF-8 message directly from the command line

thousandflicks decode <input_bmp> [output_message_file]
	# Decode a message from a BMP image, print to stdout or write to file

thousandflicks capacity <input_bmp>
	# Show the maximum message size (in bytes) that can be encoded in the image

thousandflicks info <input_bmp>
	# Show image info and encoding capacity

thousandflicks help
	# Show help and usage
```

## How to Use

1. Prepare a 24-bit uncompressed BMP image (other formats are not supported).
2. Prepare a message file (plain text or binary) or use a short message string.
3. Encode the message:
   - File: `thousandflicks encode input.bmp output.bmp message.txt`
   - Text: `thousandflicks encode-text input.bmp output.bmp "Hello world!"`
4. Share the output image. To extract the message:
   - `thousandflicks decode output.bmp` (prints to terminal)
   - `thousandflicks decode output.bmp message_out.txt` (writes to file)

## Example: Encode and Decode a Short Message

```
echo "hello world" > msg.txt
cp some_image.bmp cover.bmp
thousandflicks encode cover.bmp secret.bmp msg.txt
thousandflicks decode secret.bmp
```

## Build

```
g++ -std=c++17 -o thousandflicks src/main.cpp src/bmp.cpp src/lsb.cpp
```

## Notes
- Only 24-bit uncompressed BMP images are supported.
- The message is stored with a 4-byte (32-bit) length header.
- For best results, use images with enough capacity (see `capacity` command).

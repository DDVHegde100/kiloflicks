# Thousand Flicks

They say a picture is worth a 1000 words, well now that is actually true. Thousandflicks is a C++ command-line tool to encode and decode up to 1000 words into images using mathematical image manipulation (not metadata). 

## Usage

```
thousandflicks encode <input_image> <output_image> <message_file>
thousandflicks decode <input_image>
```

- `encode`: Encodes a message (from a text file) into an image.
- `decode`: Decodes a message from an image.

## Build

```
g++ -std=c++17 -o thousandflicks src/main.cpp
```

## Run

See usage above. No image processing is implemented yet.

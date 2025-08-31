// main.cpp

// Thousand Flicks CLI with BMP support
#include <iostream>
#include <string>
#include "bmp.h"

void print_usage() {
    std::cout << "Usage:\n";
    std::cout << "  thousandflicks encode <input_image> <output_image> <message_file>\n";
    std::cout << "  thousandflicks decode <input_image>\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }
    std::string command = argv[1];

    if (command == "encode") {
        if (argc != 5) {
            print_usage();
            return 1;
        }
        try {
            BMPImage img = load_bmp(argv[2]);
            // TODO: Encode message from argv[4] into img.data
            write_bmp(argv[3], img);
            std::cout << "[OK] Image loaded and saved (no encoding yet).\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 2;
        }
    } else if (command == "decode") {
        if (argc != 3) {
            print_usage();
            return 1;
        }
        try {
            BMPImage img = load_bmp(argv[2]);
            // TODO: Decode message from img.data
            std::cout << "[OK] Image loaded (no decoding yet).\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 2;
        }
    } else {
        print_usage();
        return 1;
    }
    return 0;
}

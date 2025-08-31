// main.cpp

// Thousand Flicks CLI with BMP support
#include <iostream>
#include <string>
#include "bmp.h"
#include "lsb.h"
#include <fstream>

void print_usage() {
    std::cout << "Usage:\n";
    std::cout << "  thousandflicks encode <input_image> <output_image> <message_file>\n";
    std::cout << "  thousandflicks encode-text <input_image> <output_image> <message_string>\n";
    std::cout << "  thousandflicks decode <input_image>\n";
    std::cout << "  thousandflicks capacity <input_image>\n";
    std::cout << "  thousandflicks info <input_image>\n";
    std::cout << "  thousandflicks help\n";
    std::cout << "\nNotes:\n  - The message is stored with a 4-byte (32-bit) length header.\n  - Only 24-bit uncompressed BMP images are supported.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }
    std::string command = argv[1];

    if (command == "encode-text") {
        if (argc != 5) {
            print_usage();
            return 1;
        }
        try {
            BMPImage img = load_bmp(argv[2]);
            std::string msgstr = argv[4];
            std::vector<uint8_t> message(msgstr.begin(), msgstr.end());
            lsb_encode(img, message);
            write_bmp(argv[3], img);
            std::cout << "[OK] Text message encoded and image saved.\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 2;
        }
    } else if (command == "help") {
        print_usage();
        return 0;
    } else if (command == "capacity") {
        if (argc != 3) {
            print_usage();
            return 1;
        }
        try {
            BMPImage img = load_bmp(argv[2]);
            std::cout << "[OK] LSB capacity: " << lsb_capacity(img) << " bytes (excluding 4-byte header)\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 2;
        }
    } else if (command == "info") {
        if (argc != 3) {
            print_usage();
            return 1;
        }
        try {
            BMPImage img = load_bmp(argv[2]);
            std::cout << "[OK] Image info: " << img.width << "x" << img.height << ", " << img.data.size() << " bytes data\n";
            std::cout << "    LSB capacity: " << lsb_capacity(img) << " bytes (excluding 4-byte header)\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 2;
        }
    } else if (command == "encode") {
        if (argc != 5) {
            print_usage();
            return 1;
        }
        try {
            BMPImage img = load_bmp(argv[2]);
            // Read message file
            std::ifstream msgfile(argv[4], std::ios::binary);
            if (!msgfile) throw std::runtime_error("Cannot open message file");
            std::vector<uint8_t> message((std::istreambuf_iterator<char>(msgfile)), std::istreambuf_iterator<char>());
            lsb_encode(img, message);
            write_bmp(argv[3], img);
            std::cout << "[OK] Message encoded and image saved.\n";
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
            // Try to decode up to 8192 bytes (arbitrary, can be increased)
            auto message = lsb_decode(img, 8192);
            std::cout << "[OK] Decoded message (raw):\n";
            std::cout.write(reinterpret_cast<const char*>(message.data()), message.size());
            std::cout << std::endl;
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

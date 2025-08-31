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
    std::cout << "  thousandflicks decode <input_image> [output_message_file]\n";
    std::cout << "  thousandflicks capacity <input_image>\n";
    std::cout << "  thousandflicks info <input_image>\n";
    std::cout << "  thousandflicks help\n";
    std::cout << "\nNotes:\n  - The message is stored with a 4-byte (32-bit) length header.\n  - Only 24-bit uncompressed BMP images are supported.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "[ERROR] No command provided.\n";
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
            if (msgstr.empty()) {
                std::cerr << "[WARN] Empty message, encoding default: 'hi'\n";
                msgstr = "hi";
            }
            std::vector<uint8_t> message(msgstr.begin(), msgstr.end());
            lsb_encode(img, message);
            write_bmp(argv[3], img);
            std::cout << "[OK] Text message encoded and image saved.\n";
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
            std::ifstream msgfile(argv[4], std::ios::binary);
            if (!msgfile) throw std::runtime_error("Cannot open message file");
            std::vector<uint8_t> message((std::istreambuf_iterator<char>(msgfile)), std::istreambuf_iterator<char>());
            if (message.empty()) {
                std::cerr << "[WARN] Empty message file, encoding default: 'hi'\n";
                message = {'h','i'};
            }
            lsb_encode(img, message);
            write_bmp(argv[3], img);
            std::cout << "[OK] Message encoded and image saved.\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 2;
        }
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
            if (command == "encode-text") {
                if (argc != 5) {
                    std::cerr << "[ERROR] Usage: thousandflicks encode-text <input_image> <output_image> <message_string>\n";
                    return 1;
                }
                try {
                    BMPImage img = load_bmp(argv[2]);
                    std::string msgstr = argv[4];
                    if (msgstr.empty()) {
                        std::cerr << "[WARN] Empty message, encoding default: 'hi'\n";
                        msgstr = "hi";
                    }
                    std::vector<uint8_t> message(msgstr.begin(), msgstr.end());
                    lsb_encode(img, message);
                    write_bmp(argv[3], img);
                    std::cout << "[OK] Text message encoded and image saved: " << argv[3] << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] " << e.what() << std::endl;
                    return 2;
                }
            } else if (command == "encode") {
                if (argc != 5) {
                    std::cerr << "[ERROR] Usage: thousandflicks encode <input_image> <output_image> <message_file>\n";
                    return 1;
                }
                try {
                    BMPImage img = load_bmp(argv[2]);
                    std::ifstream msgfile(argv[4], std::ios::binary);
                    if (!msgfile) throw std::runtime_error("Cannot open message file: " + std::string(argv[4]));
                    std::vector<uint8_t> message((std::istreambuf_iterator<char>(msgfile)), std::istreambuf_iterator<char>());
                    if (message.empty()) {
                        std::cerr << "[WARN] Empty message file, encoding default: 'hi'\n";
                        message = {'h','i'};
                    }
                    lsb_encode(img, message);
                    write_bmp(argv[3], img);
                    std::cout << "[OK] Message encoded and image saved: " << argv[3] << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] " << e.what() << std::endl;
                    return 2;
                }
            } else if (command == "decode") {
                if (argc != 3 && argc != 4) {
                    std::cerr << "[ERROR] Usage: thousandflicks decode <input_image> [output_message_file]\n";
                    return 1;
                }
                try {
                    BMPImage img = load_bmp(argv[2]);
                    auto message = lsb_decode(img, 8192);
                    if (message.empty()) {
                        std::cerr << "[ERROR] No message found or header invalid." << std::endl;
                        return 3;
                    }
                    if (argc == 4) {
                        std::ofstream out(argv[3], std::ios::binary);
                        if (!out) throw std::runtime_error("Cannot open output file: " + std::string(argv[3]));
                        out.write(reinterpret_cast<const char*>(message.data()), message.size());
                        std::cout << "[OK] Decoded message written to file: " << argv[3] << "\n";
                    } else {
                        std::cout << "[OK] Decoded message (raw):\n";
                        std::cout.write(reinterpret_cast<const char*>(message.data()), message.size());
                        std::cout << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] " << e.what() << std::endl;
                    return 2;
                }
            } else if (command == "capacity") {
                if (argc != 3) {
                    std::cerr << "[ERROR] Usage: thousandflicks capacity <input_image>\n";
                    return 1;
                }
                try {
                    BMPImage img = load_bmp(argv[2]);
                    std::cout << "[OK] LSB capacity: " << lsb_capacity(img) << " bytes (excluding 4-byte header)\n";
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] " << e.what() << std::endl;
                    return 2;
                }
            } else if (command == "info") {
                if (argc != 3) {
                    std::cerr << "[ERROR] Usage: thousandflicks info <input_image>\n";
                    return 1;
                }
                try {
                    BMPImage img = load_bmp(argv[2]);
                    std::cout << "[OK] Image info: " << img.width << "x" << img.height << ", " << img.data.size() << " bytes data\n";
                    std::cout << "    LSB capacity: " << lsb_capacity(img) << " bytes (excluding 4-byte header)\n";
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] " << e.what() << std::endl;
                    return 2;
                }
            } else if (command == "help") {
                print_usage();
                return 0;
            } else {
                std::cerr << "[ERROR] Unknown command: " << command << "\n";
                print_usage();
                return 1;
            }
            return 0;
        }

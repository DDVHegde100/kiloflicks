#include "bmp.h"
#include "lsb.h"
#include "hamming.h"
#include "prng_permute.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

void print_usage() {
    std::cout << "Thousand Flicks - Steganography Tool\n";
    std::cout << "Usage:\n";
    std::cout << "  thousandflicks decode <input_image> <output_file> [--passphrase <pass>]\n";
    std::cout << "  thousandflicks encode-text <input_image> <output_image> <message_string> [--passphrase <pass>]\n";
    std::cout << "  thousandflicks encode <input_image> <output_image> <message_file> [--passphrase <pass>]\n";
    std::cout << "  thousandflicks capacity <input_image>\n";
    std::cout << "  thousandflicks info <input_image>\n";
    std::cout << "  thousandflicks help\n";
    std::cout << "\nFeatures:\n";
    std::cout << "  - Hamming(7,4) error correction for data integrity\n";
    std::cout << "  - Optional passphrase-based PRNG permutation for security\n";
    std::cout << "  - Statistics and recovery logging\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        // No arguments - try to run Python GUI
        std::cout << "[INFO] No arguments provided, launching image upload GUI...\n";
        int result = std::system("python3 image_upload.py");
        if (result != 0) {
            std::cout << "[WARN] GUI launch failed, showing usage:\n";
            print_usage();
        }
        return 0;
    }

    std::string command = argv[1];

    if (command == "decode") {
        std::string passphrase;
        if (argc == 5 && std::string(argv[3]) == "--passphrase") {
            passphrase = argv[4];
        } else if (argc == 6 && std::string(argv[4]) == "--passphrase") {
            passphrase = argv[5];
        } else if (argc != 3 && argc != 4) {
            print_usage();
            return 1;
        }
        
        try {
            BMPImage img = load_bmp(argv[2]);
            
            // Apply passphrase permutation if provided
            if (!passphrase.empty()) {
                auto perm = prng_permutation(img.data.size(), passphrase);
                std::vector<uint8_t> unpermuted(img.data.size());
                for (size_t i = 0; i < img.data.size(); ++i) unpermuted[i] = img.data[perm[i]];
                img.data.swap(unpermuted);
            }
            
            auto message = lsb_decode(img, lsb_capacity(img));
            bool had_error = false;
            auto decoded = hamming74_decode(message, had_error);
            
            // Write output
            std::string output_file = (argc >= 4 && std::string(argv[3]) != "--passphrase") ? argv[3] : "decoded.txt";
            std::ofstream outfile(output_file, std::ios::binary);
            if (!outfile) throw std::runtime_error("Cannot create output file");
            outfile.write(reinterpret_cast<const char*>(decoded.data()), decoded.size());
            
            std::cout << "[OK] Message decoded and saved to " << output_file << "\n";
            std::cout << "     Payload size: " << decoded.size() << " bytes\n";
            if (had_error) {
                std::cout << "     [RECOVERY] Hamming ECC corrected bit errors during decode\n";
            } else {
                std::cout << "     [CLEAN] No bit errors detected\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] " << e.what() << std::endl;
            return 2;
        }
    } else if (command == "encode-text") {
        std::string passphrase;
        if (argc == 7 && std::string(argv[5]) == "--passphrase") {
            passphrase = argv[6];
        } else if (argc != 5) {
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
            auto encoded = hamming74_encode(message);
            
            // Apply passphrase permutation if provided
            if (!passphrase.empty()) {
                auto perm = prng_permutation(img.data.size(), passphrase);
                std::vector<uint8_t> permuted(img.data.size());
                for (size_t i = 0; i < img.data.size(); ++i) permuted[perm[i]] = img.data[i];
                img.data.swap(permuted);
            }
            
            lsb_encode(img, encoded);
            write_bmp(argv[3], img);
            
            std::cout << "[OK] Text message encoded and image saved to " << argv[3] << "\n";
            std::cout << "     Original message: " << message.size() << " bytes\n";
            std::cout << "     With Hamming ECC: " << encoded.size() << " bytes\n";
            std::cout << "     Image capacity: " << lsb_capacity(img) << " bytes\n";
            std::cout << "     Capacity used: " << (encoded.size() * 100.0 / lsb_capacity(img)) << "%\n";
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] " << e.what() << std::endl;
            return 2;
        }
    } else if (command == "encode") {
        std::string passphrase;
        if (argc == 6 && std::string(argv[4]) == "--passphrase") {
            passphrase = argv[5];
        } else if (argc != 5) {
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
            
            auto encoded = hamming74_encode(message);
            
            // Apply passphrase permutation if provided
            if (!passphrase.empty()) {
                auto perm = prng_permutation(img.data.size(), passphrase);
                std::vector<uint8_t> permuted(img.data.size());
                for (size_t i = 0; i < img.data.size(); ++i) permuted[perm[i]] = img.data[i];
                img.data.swap(permuted);
            }
            
            lsb_encode(img, encoded);
            write_bmp(argv[3], img);
            
            std::cout << "[OK] File message encoded and image saved to " << argv[3] << "\n";
            std::cout << "     Original file: " << message.size() << " bytes\n";
            std::cout << "     With Hamming ECC: " << encoded.size() << " bytes\n";
            std::cout << "     Image capacity: " << lsb_capacity(img) << " bytes\n";
            std::cout << "     Capacity used: " << (encoded.size() * 100.0 / lsb_capacity(img)) << "%\n";
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] " << e.what() << std::endl;
            return 2;
        }
    } else if (command == "capacity") {
        if (argc != 3) {
            print_usage();
            return 1;
        }
        try {
            BMPImage img = load_bmp(argv[2]);
            std::cout << "[OK] Image capacity: " << lsb_capacity(img) << " bytes (excluding 4-byte header)\n";
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] " << e.what() << std::endl;
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
            std::cout << "     LSB capacity: " << lsb_capacity(img) << " bytes (excluding 4-byte header)\n";
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

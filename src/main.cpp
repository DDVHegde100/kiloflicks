#include "bmp.h"
#include "lsb.h"
#include "hamming.h"
#include "prng_permute.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>

void print_banner() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                      THOUSAND FLICKS                         ║\n";
    std::cout << "║              Advanced Steganography Toolkit                  ║\n";
    std::cout << "║          Hide up to 1000 words in BMP images securely        ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void print_usage() {
    print_banner();
    std::cout << "🔧 COMMAND LINE USAGE:\n";
    std::cout << "═══════════════════════\n\n";
    
    std::cout << "📝 ENCODING:\n";
    std::cout << "  ./thousandflicks encode-text <input.bmp> <output.bmp> \"<message>\" [--passphrase <pass>]\n";
    std::cout << "  ./thousandflicks encode <input.bmp> <output.bmp> <message_file> [--passphrase <pass>]\n\n";
    
    std::cout << "🔍 DECODING:\n";
    std::cout << "  ./thousandflicks decode <encoded.bmp> [output_file] [--passphrase <pass>]\n\n";
    
    std::cout << "📊 ANALYSIS:\n";
    std::cout << "  ./thousandflicks capacity <image.bmp>    # Check how much data can be hidden\n";
    std::cout << "  ./thousandflicks info <image.bmp>        # Show image information\n";
    std::cout << "  ./thousandflicks help                    # Show this help\n\n";
    
    std::cout << "🚀 GUI MODE:\n";
    std::cout << "  ./thousandflicks                         # Launch without arguments for GUI\n";
    std::cout << "  python3 gui_app.py                      # Advanced GUI interface\n\n";
    
    std::cout << "🛡️ SECURITY FEATURES:\n";
    std::cout << "  ✓ Hamming(7,4) error correction for data integrity\n";
    std::cout << "  ✓ Passphrase-based PRNG permutation for obfuscation\n";
    std::cout << "  ✓ LSB steganography with capacity management\n";
    std::cout << "  ✓ Corruption detection and recovery logging\n\n";
    
    std::cout << "📝 EXAMPLES:\n";
    std::cout << "  ./thousandflicks encode-text input.bmp secret.bmp \"Hello World!\"\n";
    std::cout << "  ./thousandflicks encode input.bmp secret.bmp message.txt --passphrase \"mykey\"\n";
    std::cout << "  ./thousandflicks decode secret.bmp decoded.txt\n";
    std::cout << "  ./thousandflicks capacity input.bmp\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        // No arguments - try to run advanced GUI first, then fallback
        std::cout << "🚀 Launching Thousand Flicks GUI...\n";
        std::cout << "   (If GUI fails, command-line usage will be shown)\n\n";
        
        // Try advanced GUI first
        int result = std::system("python3 gui_app.py 2>/dev/null");
        if (result != 0) {
            // Fallback to simple uploader
            result = std::system("python3 image_upload.py 2>/dev/null");
            if (result != 0) {
                std::cout << "⚠️  GUI unavailable. Showing command-line usage:\n";
                print_usage();
            }
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
            
            std::cout << "\n🎉 SUCCESS! Message decoded successfully!\n";
            std::cout << "══════════════════════════════════════════\n";
            std::cout << "📄 Output file: " << output_file << "\n";
            std::cout << "📊 Payload size: " << decoded.size() << " bytes\n";
            if (had_error) {
                std::cout << "🛠️  [RECOVERY] Hamming ECC corrected bit errors during decode\n";
            } else {
                std::cout << "✅ [CLEAN] No bit errors detected - perfect integrity!\n";
            }
            std::cout << "══════════════════════════════════════════\n\n";
        } catch (const std::exception& e) {
            std::cerr << "❌ [ERROR] " << e.what() << std::endl;
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
            
            std::cout << "\n🎉 SUCCESS! Text message encoded successfully!\n";
            std::cout << "═══════════════════════════════════════════════\n";
            std::cout << "📄 Output image: " << argv[3] << "\n";
            std::cout << "📝 Original message: " << message.size() << " bytes\n";
            std::cout << "🔐 With Hamming ECC: " << encoded.size() << " bytes (+" 
                      << ((encoded.size() - message.size()) * 100.0 / message.size()) << "% overhead)\n";
            std::cout << "📊 Image capacity: " << lsb_capacity(img) << " bytes\n";
            std::cout << "💾 Capacity used: " << std::fixed << std::setprecision(1) 
                      << (encoded.size() * 100.0 / lsb_capacity(img)) << "%\n";
            if (!passphrase.empty()) {
                std::cout << "🔒 Passphrase protection: ENABLED\n";
            }
            std::cout << "═══════════════════════════════════════════════\n\n";
        } catch (const std::exception& e) {
            std::cerr << "❌ [ERROR] " << e.what() << std::endl;
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
            
            std::cout << "\n🎉 SUCCESS! File message encoded successfully!\n";
            std::cout << "══════════════════════════════════════════════\n";
            std::cout << "📄 Output image: " << argv[3] << "\n";
            std::cout << "📁 Original file: " << message.size() << " bytes\n";
            std::cout << "🔐 With Hamming ECC: " << encoded.size() << " bytes (+" 
                      << ((encoded.size() - message.size()) * 100.0 / message.size()) << "% overhead)\n";
            std::cout << "📊 Image capacity: " << lsb_capacity(img) << " bytes\n";
            std::cout << "💾 Capacity used: " << std::fixed << std::setprecision(1) 
                      << (encoded.size() * 100.0 / lsb_capacity(img)) << "%\n";
            if (!passphrase.empty()) {
                std::cout << "🔒 Passphrase protection: ENABLED\n";
            }
            std::cout << "══════════════════════════════════════════════\n\n";
        } catch (const std::exception& e) {
            std::cerr << "❌ [ERROR] " << e.what() << std::endl;
            return 2;
        }
    } else if (command == "capacity") {
        if (argc != 3) {
            print_usage();
            return 1;
        }
        try {
            BMPImage img = load_bmp(argv[2]);
            std::cout << "\n📊 IMAGE CAPACITY ANALYSIS\n";
            std::cout << "═══════════════════════════\n";
            std::cout << "🎯 Maximum storage: " << lsb_capacity(img) << " bytes (excluding 4-byte header)\n";
            std::cout << "📝 Approximate words: ~" << (lsb_capacity(img) / 5) << " words (assuming 5 chars/word)\n";
            std::cout << "📄 Text pages: ~" << (lsb_capacity(img) / 2000) << " pages (assuming 2000 chars/page)\n";
            std::cout << "═══════════════════════════\n\n";
        } catch (const std::exception& e) {
            std::cerr << "❌ [ERROR] " << e.what() << std::endl;
            return 2;
        }
    } else if (command == "info") {
        if (argc != 3) {
            print_usage();
            return 1;
        }
        try {
            BMPImage img = load_bmp(argv[2]);
            std::cout << "\n🖼️  IMAGE INFORMATION\n";
            std::cout << "══════════════════════\n";
            std::cout << "📐 Dimensions: " << img.width << " × " << img.height << " pixels\n";
            std::cout << "💾 Data size: " << img.data.size() << " bytes\n";
            std::cout << "🎯 LSB capacity: " << lsb_capacity(img) << " bytes (excluding header)\n";
            std::cout << "📊 Storage efficiency: " << std::fixed << std::setprecision(2) 
                      << (lsb_capacity(img) * 100.0 / img.data.size()) << "% of image data\n";
            std::cout << "══════════════════════\n\n";
        } catch (const std::exception& e) {
            std::cerr << "❌ [ERROR] " << e.what() << std::endl;
            return 2;
        }
    } else if (command == "help") {
        print_usage();
        return 0;
    } else {
        std::cerr << "❌ [ERROR] Unknown command: '" << command << "'\n\n";
        print_usage();
        return 1;
    }
    
    return 0;
}

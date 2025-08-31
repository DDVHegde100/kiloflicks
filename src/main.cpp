// main.cpp
// Thousand Flicks CLI skeleton
#include <iostream>
#include <string>

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
        std::cout << "[Stub] Encoding message from '" << argv[4] << "' into image '" << argv[2] << "', output: '" << argv[3] << "'\n";
        // TODO: Implement encoding
    } else if (command == "decode") {
        if (argc != 3) {
            print_usage();
            return 1;
        }
        std::cout << "[Stub] Decoding message from image '" << argv[2] << "'\n";
        // TODO: Implement decoding
    } else {
        print_usage();
        return 1;
    }
    return 0;
}

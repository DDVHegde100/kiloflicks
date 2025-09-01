# 🎭 Thousand Flicks - Advanced Steganography Toolkit

![Thousand Flicks Banner](https://img.shields.io/badge/Thousand%20Flicks-Advanced%20Steganography-brightgreen)
![C++17](https://img.shields.io/badge/C++-17-blue.svg)
![Python](https://img.shields.io/badge/Python-3.x-yellow.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

**Thousand Flicks** is a sophisticated steganography application that can hide up to 1000 words (or equivalent data) within BMP images using mathematical manipulation techniques. The tool combines **LSB (Least Significant Bit) steganography** with **Hamming(7,4) error-correcting codes** and **passphrase-based PRNG permutation** for secure, reliable data hiding.

> *"They say a picture is worth a thousand words - now that's literally true!"*

---

## ✨ Key Features

### 🔐 **Advanced Security**
- **Hamming(7,4) Error Correction**: Automatically detects and corrects single-bit errors during decode
- **Passphrase Protection**: Optional PRNG-based channel permutation for obfuscation
- **Data Integrity**: Built-in corruption detection and recovery logging

### 🎯 **User-Friendly Interface**
- **Advanced GUI**: Full-featured Python/Tkinter interface with image preview
- **Command Line**: Powerful CLI with beautiful formatted output
- **Smart Fallbacks**: Automatic GUI detection with CLI fallback

### 🛠️ **Technical Excellence**
- **LSB Steganography**: Robust least significant bit manipulation
- **Capacity Management**: Automatic capacity checking and overflow protection
- **Cross-Platform**: Works on macOS, Linux, and Windows
- **BMP Support**: Optimized for 24-bit uncompressed BMP images

---

## 🚀 Quick Start

### Prerequisites
```bash
# C++ Compiler (C++17 support required)
g++ --version

# Python 3 (for GUI)
python3 --version

# Optional: PIL/Pillow for advanced GUI features
pip3 install Pillow
```

### 📦 Installation
```bash
# Clone the repository
git clone https://github.com/DDVHegde100/thousandflicks.git
cd thousandflicks

# Compile the application
g++ -std=c++17 -I. -o thousandflicks src/main.cpp src/bmp.cpp src/lsb.cpp src/hamming.cpp src/prng_permute.cpp

# Make executable
chmod +x thousandflicks
```

### 🎯 First Use
```bash
# Launch GUI mode (recommended for beginners)
./thousandflicks

# Or use advanced GUI directly
python3 gui_app.py

# Command line help
./thousandflicks help
```

---

## 📋 Usage Guide

### 🖥️ **GUI Mode (Recommended)**

The GUI provides three main tabs:

#### 📝 **Encode Tab**
1. **Select Images**: Choose input BMP and specify output location
2. **Enter Message**: Type directly or load from file
3. **Security Options**: Optionally enable passphrase protection
4. **Encode**: Click "Encode Message" to hide your data

#### 🔍 **Decode Tab**
1. **Select Image**: Choose the encoded BMP file
2. **Security**: Enter passphrase if the image is protected
3. **Output Options**: Display in window or save to file
4. **Decode**: Extract the hidden message

#### ℹ️ **Info Tab**
1. **Analyze Images**: Check capacity and view image properties
2. **Preview**: See thumbnail preview of selected images
3. **Statistics**: View detailed capacity and storage information

### 💻 **Command Line Interface**

#### 📝 **Encoding Messages**
```bash
# Encode text message
./thousandflicks encode-text input.bmp output.bmp "Your secret message here"

# Encode from file
./thousandflicks encode input.bmp output.bmp message.txt

# With passphrase protection
./thousandflicks encode-text input.bmp output.bmp "Secret!" --passphrase "mykey123"
```

#### 🔍 **Decoding Messages**
```bash
# Decode to default file (decoded.txt)
./thousandflicks decode encoded.bmp

# Decode to specific file
./thousandflicks decode encoded.bmp my_message.txt

# Decode with passphrase
./thousandflicks decode encoded.bmp output.txt --passphrase "mykey123"
```

#### 📊 **Image Analysis**
```bash
# Check storage capacity
./thousandflicks capacity image.bmp

# View image information
./thousandflicks info image.bmp
```

---

## 🎬 Complete Workflow Example

### 🎯 **End-to-End Demo**
```bash
# 1. Create a test message
echo "This is my secret message for Thousand Flicks demo!" > secret.txt

# 2. Check image capacity
./thousandflicks capacity test_medium.bmp
# Output: 📊 Maximum storage: 179,996 bytes (~35,999 words)

# 3. Encode with passphrase protection
./thousandflicks encode test_medium.bmp encoded_secret.bmp secret.txt --passphrase "demo123"
# Output: 🎉 SUCCESS! File message encoded successfully!

# 4. Verify encoding worked
./thousandflicks info encoded_secret.bmp
# Output: 🖼️ IMAGE INFORMATION with capacity details

# 5. Decode the message
./thousandflicks decode encoded_secret.bmp decoded_message.txt --passphrase "demo123"
# Output: 🎉 SUCCESS! Message decoded successfully!

# 6. Verify message integrity
diff secret.txt decoded_message.txt
# No output = perfect match!
```

---

## 🏗️ Technical Architecture

### 🔧 **Core Components**

#### **1. BMP Image Handler** (`src/bmp.h`, `src/bmp.cpp`)
- Loads and writes 24-bit uncompressed BMP files
- Robust header parsing and validation
- Cross-platform byte order handling

#### **2. LSB Steganography Engine** (`src/lsb.h`, `src/lsb.cpp`)
- Least Significant Bit manipulation
- Automatic capacity calculation
- 32-bit message length headers
- Overflow protection

#### **3. Hamming Error Correction** (`src/hamming.h`, `src/hamming.cpp`)
- Hamming(7,4) systematic encoding
- Single-bit error detection and correction
- Corruption recovery logging
- ~75% data overhead for reliability

#### **4. PRNG Permutation** (`src/prng_permute.h`, `src/prng_permute.cpp`)
- Passphrase-based seed generation
- Channel order randomization
- Reversible permutation algorithms
- Additional security layer

#### **5. Command Line Interface** (`src/main.cpp`)
- Beautiful formatted output with Unicode symbols
- Comprehensive error handling
- Statistics and progress reporting
- Smart GUI fallback system

### 🔄 **Data Flow**

```
Input Message → Hamming Encode → LSB Embed → [Optional PRNG] → Output Image
                      ↓
              75% size increase for error correction

Output Image → [Optional PRNG⁻¹] → LSB Extract → Hamming Decode → Original Message
                                                      ↓
                                            Error detection & correction
```

### 📊 **Capacity Calculation**
```cpp
// Each pixel has 3 channels (RGB), each can store 1 bit
// Reserve 32 bits (4 bytes) for message length header
capacity = (width × height × 3) - 32 bits
         = (width × height × 3 - 32) / 8 bytes
```

---

## 🧪 Testing & Validation

### ✅ **Built-in Test Suite**
```bash
# Compile and run Hamming ECC tests
g++ -std=c++17 -o test_hamming test_hamming.cpp src/hamming.cpp
./test_hamming

# Create test images
python3 create_test_image.py
```

### 🔍 **Manual Testing Workflow**
```bash
# 1. Create test message
echo "Testing Thousand Flicks steganography with error correction!" > test.txt

# 2. Check capacity
./thousandflicks capacity test_medium.bmp

# 3. Encode with passphrase
./thousandflicks encode test_medium.bmp encoded.bmp test.txt --passphrase "test123"

# 4. Decode and verify
./thousandflicks decode encoded.bmp decoded.txt --passphrase "test123"

# 5. Compare original and decoded
diff test.txt decoded.txt
```

---

## 🐛 Troubleshooting

### ❓ **Common Issues**

#### **"Cannot open BMP file" Error**
```bash
# Check file exists and is readable
ls -la your_image.bmp
file your_image.bmp

# Ensure it's a valid 24-bit BMP
./thousandflicks info your_image.bmp
```

#### **"Message too large for image capacity" Error**
```bash
# Check capacity first
./thousandflicks capacity your_image.bmp

# Remember: Hamming encoding increases size by 75%
# A 100-byte message becomes ~175 bytes after encoding
```

#### **GUI Won't Start**
```bash
# Check Python 3 installation
python3 --version

# Install required packages (optional for basic functionality)
pip3 install Pillow

# Try simple GUI first
python3 image_upload.py

# Fall back to command line
./thousandflicks help
```

---

## 🤝 Contributing

We welcome contributions! Areas for improvement:

- **Additional Image Formats**: PNG, JPEG support
- **Enhanced GUI**: More user-friendly interfaces
- **Performance Optimization**: Faster algorithms
- **Mobile Support**: iOS/Android apps
- **Documentation**: More examples and tutorials

---

## 📄 License

This project is licensed under the MIT License.

## 🙏 Acknowledgments

- **Richard Hamming** for error-correcting codes
- **Microsoft** for the BMP format specification  
- **Open Source Community** for inspiration and tools

---

*Made with ❤️ for secure and private communication.*

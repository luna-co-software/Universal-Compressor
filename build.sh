#!/bin/bash

# Universal Compressor Build Script
set -e

echo "Building Universal Compressor..."

# Clean previous build
cd build
make clean
cd ..

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with half the CPU cores
make -j$(($(nproc) / 2))

echo "Build complete!"
echo "Plugin binaries are in the build directory:"
echo "- VST3: build/UniversalCompressor_artefacts/Release/VST3/"
echo "- AU: build/UniversalCompressor_artefacts/Release/AU/"
echo "- Standalone: build/UniversalCompressor_artefacts/Release/Standalone/"

# Optional: Install to system plugin directories
read -p "Install plugins to system directories? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
    echo "Installing plugins..."
    
    # Install VST3
    if [ -d "UniversalCompressor_artefacts/Release/VST3/" ]; then
        mkdir -p ~/.vst3
        cp -r UniversalCompressor_artefacts/Release/VST3/* ~/.vst3/
        echo "VST3 installed to ~/.vst3"
    fi
    
    # Install AU (macOS only)
    if [[ "$OSTYPE" == "darwin"* ]] && [ -d "UniversalCompressor_artefacts/Release/AU/" ]; then
        sudo cp -r UniversalCompressor_artefacts/Release/AU/* /Library/Audio/Plug-Ins/Components/
        echo "AU installed to /Library/Audio/Plug-Ins/Components/"
    fi
    
    echo "Installation complete!"
fi
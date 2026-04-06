#!/bin/sh

# Create the output directory
mkdir -p bin/Release

# Compile and link main.cpp
echo "Building Xasma..."
g++ -Wall -fexceptions -O2 main.cpp -o bin/Release/Xasma -s

echo "Build complete!"

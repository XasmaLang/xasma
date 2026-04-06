def build_xasma [] {
    # Ensure the output directory exists
    mkdir bin/Release

    # Compile and link main.cpp
    # Removed .exe extension and ensured forward slashes
    print "Building Xasma..."
    g++ -Wall -fexceptions -O2 main.cpp -o bin/Release/Xasma -s
    
    print "Build complete!"
}

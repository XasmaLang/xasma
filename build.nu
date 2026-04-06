# Ensure the output directory exists
mkdir bin\Release

# Compile and link main.cpp directly
print "Building Xasma..."
g++ -Wall -fexceptions -O2 main.cpp -o bin\Release\Xasma.exe -s

print "Build complete!"

import subprocess
import os
import sys
import platform

def build_xasma():
    # Define paths
    out_dir = os.path.join("bin", "Release")
    source_file = "main.cpp"
    
    # Detect OS and set executable name
    exe_name = "Xasma.exe" if platform.system() == "Windows" else "Xasma"
    target_path = os.path.join(out_dir, exe_name)

    # Ensure output directory exists
    os.makedirs(out_dir, exist_ok=True)

    # Command to execute
    cmd = [
        "g++', 
        "-Wall", 
        "-fexceptions", 
        "-O2", 
        source_file, 
        "-o", 
        target_path, 
        "-s"
    ]

    print(f"Building {exe_name}...")
    
    try:
        # Run the compilation command
        subprocess.run(cmd, check=True)
        print("Build complete!")
    except subprocess.CalledProcessError as e:
        print(f"Build failed with error: {e}")
    except FileNotFoundError:
        print("Error: g++ not found. Please ensure it is installed and in your PATH.")

if __name__ == "__main__":
    build_xasma()

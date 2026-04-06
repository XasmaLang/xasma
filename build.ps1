function build-xasma {
    # Define directories and base name
    $outDir = "bin/Release"
    $targetName = "Xasma"

    # Detect OS and set extension
    if ($IsWindows) {
        $exeName = "$targetName.exe"
    } else {
        $exeName = $targetName
    }

    # Ensure output directory exists
    if (!(Test-Path $outDir)) {
        New-Item -ItemType Directory -Path $outDir | Out-Null
    }

    # Compile and link
    Write-Host "Building $exeName..."
    g++ -Wall -fexceptions -O2 main.cpp -o "$outDir/$exeName" -s
    
    Write-Host "Build complete!"
}

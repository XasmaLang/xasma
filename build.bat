@echo off
:: Create the output directory
if not exist "bin\Release" mkdir "bin\Release"

:: Compile and link main.cpp
echo Building Xasma...
g++ -Wall -fexceptions -O2 main.cpp -o "bin\Release\Xasma.exe" -s

echo Build complete!
pause

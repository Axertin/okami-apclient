#!/bin/bash
# Formats all C++ files in the project

set -e

if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format not found"
    echo "Install instructions:"
    echo "  Windows: winget install LLVM.LLVM (or included with Visual Studio)"
    echo "  macOS: brew install clang-format"
    echo "  Linux: sudo apt install clang-format"
    echo ""
    echo "Windows users: Run this script in Git Bash or WSL"
    exit 1
fi

echo "Formatting C++ files..."

# Find and format all C++ files
find src include tests -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" 2>/dev/null | while read -r file; do
    echo "Formatting $file"
    clang-format -i "$file"
done

echo "Formatting complete!"

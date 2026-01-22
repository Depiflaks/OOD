#!/bin/bash

echo "Counting lines of code in current directory and subdirectories..."
echo ""

# Для C++ файлов
cpp_lines=$(find . -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec cat {} \; | wc -l)
echo "C++ files: $cpp_lines lines"

# Для всех текстовых файлов
total_lines=$(find . -type f -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.txt" -o -name "*.md" | xargs wc -l | tail -1 | awk '{print $1}')
echo "Total lines in all text files: $total_lines"

# Подробный вывод по каждому типу файлов
echo ""
echo "Detailed breakdown:"
find . -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print0 | xargs -0 wc -l

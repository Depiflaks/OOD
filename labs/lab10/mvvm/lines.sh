#!/bin/bash

echo "Counting lines of code in current directory and subdirectories..."
echo ""

cpp_lines=$(find . -type f \( -name "*.go" \) -exec cat {} \; | wc -l)
echo "golang files: $cpp_lines lines"

# Для всех текстовых файлов
total_lines=$(find . -type f -name "*.go" | xargs wc -l | tail -1 | awk '{print $1}')
echo "Total lines in all text files: $total_lines"

# Подробный вывод по каждому типу файлов
echo ""
echo "Detailed breakdown:"
find . -type f \( -name "*.go" \) -print0 | xargs -0 wc -l

#!/bin/bash

echo "============================================"
echo "OC32 HAL Config Generator - Build Script"
echo "============================================"
echo ""

# Check Python
if ! command -v python3 >/dev/null 2>&1 && ! command -v python >/dev/null 2>&1; then
    echo "ERROR: Python not found. Please install Python 3.8+."
    read -p "Press Enter to exit..."
    exit 1
fi

PYTHON_CMD=$(command -v python3 2>/dev/null || command -v python)
PYTHON_VERSION=$($PYTHON_CMD --version 2>&1)
echo "Python: $PYTHON_VERSION"

# Check pip
if ! $PYTHON_CMD -m pip --version >/dev/null 2>&1; then
    echo "pip not found, trying to install..."
    if command -v apt >/dev/null 2>&1; then
        sudo apt update && sudo apt install -y python3-pip || true
    elif command -v pip3 >/dev/null 2>&1; then
        :
    else
        echo "Warning: pip not available. Trying to continue..."
    fi
fi

# Check PyInstaller
if ! $PYTHON_CMD -m pip show pyinstaller >/dev/null 2>&1; then
    echo "Installing PyInstaller..."
    $PYTHON_CMD -m pip install --user pyinstaller || $PYTHON_CMD -m pip install pyinstaller || pip install --user pyinstaller || true
fi

# Clean old build
[ -d "build" ] && rm -rf "build"
[ -d "dist" ] && rm -rf "dist"

# Build Linux executable
echo "Building standalone executable for Linux..."
$PYTHON_CMD -m PyInstaller --onefile --windowed \
    --name "OC32_HAL_Config_Generator" \
    --add-data "chips:chips" \
    --add-data "hal:hal" \
    hal_conf_generator.py --noconfirm

echo ""

if [ -f "dist/OC32_HAL_Config_Generator" ]; then
    TARGET_DIR="/home/fro/oc32/linux/bin"
    mkdir -p "$TARGET_DIR"
    cp "dist/OC32_HAL_Config_Generator" "$TARGET_DIR/"
    echo "Copied to $TARGET_DIR/OC32_HAL_Config_Generator"
fi

echo "Linux build complete: dist/OC32_HAL_Config_Generator"

# Build Windows executable with Wine if available
WINE_PYTHON="/backup/wine/python/drive_c/users/fro/AppData/Local/Programs/Python/Python315/python.exe"
if command -v wine >/dev/null 2>&1 && [ -f "$WINE_PYTHON" ]; then
    echo ""
    echo "Building standalone executable for Windows (Wine)..."
    export WINEPREFIX=/backup/wine/python
    wine C:/users/fro/AppData/Local/Programs/Python/Python315/python.exe -m PyInstaller --onefile --windowed \
        --name "OC32_HAL_Config_Generator" \
        --add-data "chips;chips" \
        --add-data "hal;hal" \
        hal_conf_generator.py --noconfirm
    echo ""
    echo "Windows build complete: dist/OC32_HAL_Config_Generator.exe"
    if [ -f "dist/OC32_HAL_Config_Generator.exe" ]; then
        TARGET_DIR="/home/fro/oc32/windows/bin"
        mkdir -p "$TARGET_DIR"
        cp "dist/OC32_HAL_Config_Generator.exe" "$TARGET_DIR/"
        echo "Copied to $TARGET_DIR/OC32_HAL_Config_Generator.exe"
    fi
else
    echo ""
    echo "Wine or Windows Python not found, skipping Windows build."
fi

echo ""
echo "============================================"
echo "Build complete!"
echo "Linux:   dist/OC32_HAL_Config_Generator"
echo "Windows: dist/OC32_HAL_Config_Generator.exe"
echo "============================================"

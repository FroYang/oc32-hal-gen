#!/bin/bash
# Build the CustomTkinter OC32 HAL Config Generator (customtk/ + gen.py + ui.py)
# into standalone executables: Linux binary and (via Wine) Windows .exe.
set -e
cd "$(dirname "$0")"

echo "============================================"
echo "OC32 HAL Config Generator (CustomTkinter)"
echo "Entry point: customtk/ui.py  (imports gen.py)"
echo "============================================"
echo ""

# ---- Python ----
if ! command -v python3 >/dev/null 2>&1 && ! command -v python >/dev/null 2>&1; then
    echo "ERROR: Python not found. Please install Python 3.8+."
    read -p "Press Enter to exit..."
    exit 1
fi
PYTHON_CMD=$(command -v python3 2>/dev/null || command -v python)
echo "Python: $($PYTHON_CMD --version 2>&1)"

# ---- Ensure pip / PyInstaller / customtkinter are present ----
$PYTHON_CMD -m pip --version >/dev/null 2>&1 || { echo "pip missing; aborting."; exit 1; }
if ! $PYTHON_CMD -m pip show pyinstaller >/dev/null 2>&1; then
    echo "Installing PyInstaller..."
    $PYTHON_CMD -m pip install --user pyinstaller || $PYTHON_CMD -m pip install pyinstaller
fi
if ! $PYTHON_CMD -c "import customtkinter" >/dev/null 2>&1; then
    echo "Installing customtkinter (required at build time so PyInstaller can bundle it)..."
    $PYTHON_CMD -m pip install --user customtkinter || $PYTHON_CMD -m pip install customtkinter
fi

# ---- Clean ----
rm -rf build_ctk dist_ctk
rm -rf build

# ---- Linux build ----
echo ""
echo "Building Linux onefile executable..."
$PYTHON_CMD -m PyInstaller --onefile --windowed \
    --distpath dist_ctk \
    --workpath build_ctk \
    --name "OC32_HAL_Config_Generator" \
    --add-data "chips:chips" \
    --add-data "hal:hal" \
    --add-data "customtk:customtk" \
    customtk/ui.py --noconfirm

if [ -f "dist_ctk/OC32_HAL_Config_Generator" ]; then
    mkdir -p /home/fro/oc32/linux/bin
    cp "dist_ctk/OC32_HAL_Config_Generator" /home/fro/oc32/linux/bin/
    echo "Copied Linux binary to /home/fro/oc32/linux/bin/OC32_HAL_Config_Generator"
else
    echo "Linux build did not produce an output file."; exit 1
fi

# ---- Windows build (cross-compile with Wine if available) ----
echo ""
WINE_PYTHON="/backup/wine/win10/drive_c/Python314/python.exe"
if command -v wine >/dev/null 2>&1 && [ -f "$WINE_PYTHON" ]; then
    echo "Building Windows onefile executable via Wine..."
    export WINEPREFIX=/backup/wine/win10
    wine C:/Python314/python.exe -m PyInstaller --onefile --windowed \
        --name "OC32_HAL_Config_Generator" \
        --add-data "chips;chips" \
        --add-data "hal;hal" \
        --add-data "customtk;customtk" \
        customtk/ui.py --noconfirm
    if [ -f "dist/OC32_HAL_Config_Generator.exe" ]; then
        mkdir -p /home/fro/oc32/windows/bin
        cp "dist/OC32_HAL_Config_Generator.exe" /home/fro/oc32/windows/bin/
        echo "Copied Windows exe to /home/fro/oc32/windows/bin/OC32_HAL_Config_Generator.exe"
    else
        echo "Windows build did not produce an output file."
    fi
else
    echo "Wine / Windows Python not found; skipping the Windows build."
    echo "(To build the .exe, run the Windows steps on a Windows host instead.)"
fi

echo ""
echo "============================================"
echo "CustomTkinter build complete!"
echo "  Linux:   dist_ctk/OC32_HAL_Config_Generator"
echo "  Windows: dist/OC32_HAL_Config_Generator.exe   (if Wine available)"
echo "============================================"

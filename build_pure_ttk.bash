#!/bin/bash
# Build the pure tkinter+ttk OC32 HAL Config Generator (pure_ttk/ + gen.py + ui.py)
# into standalone executables: Linux binary and (via Wine) Windows .exe.
#
# pure_ttk 版只依赖 Python 标准库（tkinter/ttk）与 PyInstaller，
# 无需 customtkinter。因此产物更小、启动解包更快。
set -e
cd "$(dirname "$0")"

echo "============================================"
echo "OC32 HAL Config Generator (pure tkinter+ttk)"
echo "Entry point: pure_ttk/ui.py  (imports gen.py)"
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

# ---- Ensure pip / PyInstaller present（纯标准库版不需要装 customtkinter） ----
$PYTHON_CMD -m pip --version >/dev/null 2>&1 || { echo "pip missing; aborting."; exit 1; }
if ! $PYTHON_CMD -m pip show pyinstaller >/dev/null 2>&1; then
    echo "Installing PyInstaller..."
    $PYTHON_CMD -m pip install --user pyinstaller || $PYTHON_CMD -m pip install pyinstaller
fi

# ---- Clean ----
rm -rf build_pure_ttk dist_pure_ttk
rm -rf build dist

# ---- Linux build ----
echo ""
echo "Building Linux onefile executable..."
$PYTHON_CMD -m PyInstaller --onefile --windowed \
    --distpath dist_pure_ttk \
    --workpath build_pure_ttk \
    --name "OC32_HAL_Config_Generator" \
    --add-data "chips:chips" \
    --add-data "hal:hal" \
    --add-data "hal/doc:hal/doc" \
    --add-data "pure_ttk:pure_ttk" \
    pure_ttk/ui.py --noconfirm

if [ -f "dist_pure_ttk/OC32_HAL_Config_Generator" ]; then
    mkdir -p /home/fro/oc32/linux/bin
    cp "dist_pure_ttk/OC32_HAL_Config_Generator" /home/fro/oc32/linux/bin/
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
        --add-data "hal/doc;hal/doc" \
        --add-data "pure_ttk;pure_ttk" \
        pure_ttk/ui.py --noconfirm
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
echo "pure tkinter+ttk build complete!"
echo "  Linux:   dist_pure_ttk/OC32_HAL_Config_Generator"
echo "  Windows: dist/OC32_HAL_Config_Generator.exe   (if Wine available)"
echo "============================================"

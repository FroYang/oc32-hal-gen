Write-Host "============================================"
Write-Host "OC32 HAL Config Generator (CustomTkinter)"
Write-Host "Entry point: customtk/ui.py  (imports gen.py)"
Write-Host "============================================"
Write-Host ""

# Check Python
$pythonVersion = python --version 2>&1 | Out-String
if (-not $pythonVersion) {
    Write-Host "ERROR: Python not found. Please install Python 3.8+." -ForegroundColor Red
    pause
    exit 1
}
Write-Host "Python: $pythonVersion.Trim()"

# Check PyInstaller
if (-not (pip show pyinstaller 2>$null)) {
    Write-Host "Installing PyInstaller..."
    pip install pyinstaller
}

# Check customtkinter (required at build time so PyInstaller can bundle it)
if (-not (python -c "import customtkinter" 2>$null)) {
    Write-Host "Installing customtkinter..."
    pip install customtkinter
}

# Clean old build (ctk variant)
if (Test-Path "build_ctk")  { Remove-Item -Recurse -Force "build_ctk"  }
if (Test-Path "dist_ctk")  { Remove-Item -Recurse -Force "dist_ctk"  }
if (Test-Path "build")     { Remove-Item -Recurse -Force "build"     }
if (Test-Path "dist")      { Remove-Item -Recurse -Force "dist"      }

# Build Windows onefile exe
Write-Host "Building Windows standalone executable (CustomTkinter)..."
python -m PyInstaller --onefile --windowed `
    --distpath "dist_ctk" `
    --workpath "build_ctk" `
    --name "OC32_HAL_Config_Generator" `
    --add-data "chips;chips" `
    --add-data "hal;hal" `
    --add-data "customtk;customtk" `
    customtk/ui.py --noconfirm

if (Test-Path "dist_ctk/OC32_HAL_Config_Generator.exe") {
    if (-not (Test-Path "D:\nsis\oc32\bin")) { New-Item -ItemType Directory -Path "D:\nsis\oc32\bin" -Force | Out-Null }
    Copy-Item "./dist_ctk/OC32_HAL_Config_Generator.exe" "D:\nsis\oc32\bin\"
    Write-Host "Copied to D:\nsis\oc32\bin\OC32_HAL_Config_Generator.exe"
} else {
    Write-Host "Windows build did not produce an output file." -ForegroundColor Red
}

Write-Host ""
Write-Host "============================================"
Write-Host "CustomTkinter build complete!"
Write-Host "Executable: dist_ctk\OC32_HAL_Config_Generator.exe"
Write-Host "============================================"

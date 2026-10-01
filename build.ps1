Write-Host "============================================"
Write-Host "OC32 HAL Config Generator - Build Script"
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
$pyinstaller = pip show pyinstaller 2>$null
if (-not $pyinstaller) {
    Write-Host "Installing PyInstaller..."
    pip install pyinstaller
}

# Clean old build
if (Test-Path "build")  { Remove-Item -Recurse -Force "build"  }
if (Test-Path "dist")  { Remove-Item -Recurse -Force "dist"  }

# Build exe
Write-Host "Building standalone executable..."
python -m PyInstaller --onefile --windowed `
    --name "OC32_HAL_Config_Generator" `
    --add-data "chips;chips" `
    --add-data "hal;hal" `
    hal_conf_generator.py --noconfirm

Write-Host ""
Write-Host "============================================"
Write-Host "Build complete!"
Write-Host "Executable: dist\OC32_HAL_Config_Generator.exe"
Write-Host "============================================"
pause

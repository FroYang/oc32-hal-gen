Write-Host "============================================"
Write-Host "OC32 HAL Config Generator (pure tkinter+ttk)"
Write-Host "Entry point: pure_ttk/ui.py  (imports gen.py)"
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

# Check PyInstaller（纯标准库版，无需装 customtkinter）
if (-not (pip show pyinstaller 2>$null)) {
    Write-Host "Installing PyInstaller..."
    pip install pyinstaller
}

# Clean old build (pure_ttk variant)
if (Test-Path "build_pure_ttk") { Remove-Item -Recurse -Force "build_pure_ttk" }
if (Test-Path "dist_pure_ttk")  { Remove-Item -Recurse -Force "dist_pure_ttk" }
if (Test-Path "build")          { Remove-Item -Recurse -Force "build" }
if (Test-Path "dist")           { Remove-Item -Recurse -Force "dist" }

# Build Windows onefile exe
Write-Host "Building Windows standalone executable (pure tkinter+ttk)..."
python -m PyInstaller --onefile --windowed `
    --distpath "dist_pure_ttk" `
    --workpath "build_pure_ttk" `
    --name "OC32_HAL_Config_Generator" `
    --add-data "chips;chips" `
    --add-data "hal;hal" `
    --add-data "hal\doc;hal\doc" `
    --add-data "pure_ttk;pure_ttk" `
    pure_ttk/ui.py --noconfirm

if (Test-Path "dist_pure_ttk/OC32_HAL_Config_Generator.exe") {
    if (-not (Test-Path "D:\nsis\oc32\bin")) { New-Item -ItemType Directory -Path "D:\nsis\oc32\bin" -Force | Out-Null }
    Copy-Item "./dist_pure_ttk/OC32_HAL_Config_Generator.exe" "D:\nsis\oc32\bin\"
    Write-Host "Copied to D:\nsis\oc32\bin\OC32_HAL_Config_Generator.exe"
} else {
    Write-Host "Windows build did not produce an output file." -ForegroundColor Red
}

Write-Host ""
Write-Host "============================================"
Write-Host "pure tkinter+ttk build complete!"
Write-Host "Executable: dist_pure_ttk\OC32_HAL_Config_Generator.exe"
Write-Host "============================================"

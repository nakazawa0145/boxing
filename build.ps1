#!/usr/bin/env pwsh
# build.ps1 — BoxingMod build script
# Run on Windows with Android NDK installed.
# Requires: qpm-rust, cmake, ninja, Android NDK r25+

param(
    [switch]$Clean,
    [switch]$Package
)

$ErrorActionPreference = "Stop"

$MOD_ID      = "BoxingMod"
$MOD_VERSION = "1.0.0"
$NDK_VERSION = "r25c"

Write-Host "=== BoxingMod Build Script ===" -ForegroundColor Cyan

# ── Restore dependencies ──────────────────────────────────────────────────────
Write-Host "[1/4] Restoring QPM dependencies..." -ForegroundColor Yellow
qpm-rust restore
if ($LASTEXITCODE -ne 0) { throw "QPM restore failed" }

# ── Clean ─────────────────────────────────────────────────────────────────────
if ($Clean -and (Test-Path "build")) {
    Write-Host "[*] Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "build"
}

# ── CMake configure ───────────────────────────────────────────────────────────
Write-Host "[2/4] Configuring CMake..." -ForegroundColor Yellow

$NDK_HOME = $env:ANDROID_NDK_HOME
if (-not $NDK_HOME) {
    # Try common paths
    $candidates = @(
        "$env:LOCALAPPDATA\Android\Sdk\ndk\25.2.9519653",
        "C:\Android\ndk\$NDK_VERSION"
    )
    foreach ($c in $candidates) {
        if (Test-Path $c) { $NDK_HOME = $c; break }
    }
}
if (-not $NDK_HOME) { throw "ANDROID_NDK_HOME not set and NDK not found" }

$TOOLCHAIN = "$NDK_HOME/build/cmake/android.toolchain.cmake"

cmake -B build -G Ninja `
    -DCMAKE_BUILD_TYPE=RelWithDebInfo `
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" `
    -DANDROID_ABI=arm64-v8a `
    -DANDROID_PLATFORM=android-29 `
    -DANDROID_STL=c++_static `
    -DMOD_ID="$MOD_ID" `
    -DVERSION="$MOD_VERSION"

if ($LASTEXITCODE -ne 0) { throw "CMake configure failed" }

# ── Build ─────────────────────────────────────────────────────────────────────
Write-Host "[3/4] Building shared library..." -ForegroundColor Yellow
cmake --build build --config RelWithDebInfo -j $([Environment]::ProcessorCount)
if ($LASTEXITCODE -ne 0) { throw "Build failed" }

Write-Host "[OK] libBoxingMod.so built successfully" -ForegroundColor Green

# ── Package QMOD ──────────────────────────────────────────────────────────────
if ($Package) {
    Write-Host "[4/4] Packaging QMOD..." -ForegroundColor Yellow

    $qmodDir = "qmod_tmp"
    if (Test-Path $qmodDir) { Remove-Item -Recurse -Force $qmodDir }
    New-Item -ItemType Directory $qmodDir | Out-Null

    Copy-Item "mod.json"                          "$qmodDir/mod.json"
    Copy-Item "build/libBoxingMod.so"             "$qmodDir/libBoxingMod.so"

    $outFile = "$MOD_ID-$MOD_VERSION.qmod"
    if (Test-Path $outFile) { Remove-Item $outFile }

    Compress-Archive -Path "$qmodDir/*" -DestinationPath "$outFile" -Force
    Remove-Item -Recurse -Force $qmodDir

    Write-Host "[OK] Package: $outFile" -ForegroundColor Green
} else {
    Write-Host "[4/4] Skipping packaging (use -Package to create .qmod)" -ForegroundColor Gray
}

Write-Host "=== Done ===" -ForegroundColor Cyan

# SPDX-License-Identifier: GPL-3.0-only
[CmdletBinding()]
param(
    [string]$QtDir = "C:\Qt\6.12.0\mingw_64",
    [string]$MingwDir = "C:\Qt\Tools\mingw1310_64",
    [string]$NinjaDir = "C:\Qt\Tools\Ninja",
    [string]$BuildDir = "build",
    [ValidateSet("Release", "RelWithDebInfo", "Debug")]
    [string]$Config = "Release",
    [switch]$SkipTests
)

$ErrorActionPreference = "Stop"
$root = Split-Path $PSScriptRoot -Parent

$gcc = Join-Path $MingwDir "bin\gcc.exe"
$gxx = Join-Path $MingwDir "bin\g++.exe"
$ninja = Join-Path $NinjaDir "ninja.exe"
$qmake = Join-Path $QtDir "bin\qmake.exe"

foreach ($required in @($gcc, $gxx, $ninja, $qmake)) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "找不到构建工具：$required"
    }
}

$cmake = (Get-Command cmake.exe -ErrorAction SilentlyContinue).Source
if (-not $cmake) {
    throw "找不到 cmake.exe，请先安装 CMake 并加入 PATH。"
}
$ctest = Join-Path (Split-Path $cmake -Parent) "ctest.exe"

$env:Path = "$QtDir\bin;$MingwDir\bin;$NinjaDir;$env:Path"
$buildPath = Join-Path $root $BuildDir

& $cmake -S $root -B $buildPath -G Ninja `
    "-DCMAKE_BUILD_TYPE=$Config" `
    "-DCMAKE_PREFIX_PATH=$QtDir" `
    "-DCMAKE_C_COMPILER=$gcc" `
    "-DCMAKE_CXX_COMPILER=$gxx" `
    "-DCMAKE_MAKE_PROGRAM=$ninja"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& $cmake --build $buildPath --parallel
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if (-not $SkipTests) {
    & $ctest --test-dir $buildPath --output-on-failure
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

Write-Host ""
Write-Host "构建完成："
Write-Host "  $(Join-Path $buildPath 'Argon2PoW-Challenge.exe')"
Write-Host "  $(Join-Path $buildPath 'Argon2PoW-Solver.exe')"

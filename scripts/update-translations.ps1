# SPDX-License-Identifier: GPL-3.0-only
[CmdletBinding()]
param([string]$QtDir = "C:\Qt\6.12.0\mingw_64")

$ErrorActionPreference = "Stop"
$root = Split-Path $PSScriptRoot -Parent
$lupdate = Join-Path $QtDir "bin\lupdate.exe"
if (-not (Test-Path -LiteralPath $lupdate)) { throw "找不到 lupdate：$lupdate" }
& $lupdate (Join-Path $root "src") -no-obsolete -source-language zh_CN -target-language en `
    -ts (Join-Path $root "translations\argon2pow_en.ts")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

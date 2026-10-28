# SPDX-License-Identifier: GPL-3.0-only
[CmdletBinding()]
param([string]$DistDir = "dist")

$ErrorActionPreference = "Stop"
$root = Split-Path $PSScriptRoot -Parent
$distPath = [IO.Path]::GetFullPath((Join-Path $root $DistDir))
$shots = Join-Path $root "build\package-smoke"
New-Item -ItemType Directory -Force -Path $shots | Out-Null
$originalPath = $env:Path
$originalPlugins = $env:QT_PLUGIN_PATH
$originalPlatform = $env:QT_QPA_PLATFORM
$originalPlatformPlugins = $env:QT_QPA_PLATFORM_PLUGIN_PATH
try {
    # Deliberately exclude the developer's Qt/compiler installation from PATH.
    $env:Path = "$env:SystemRoot\System32;$env:SystemRoot"
    $env:QT_QPA_PLATFORM = "windows"
    foreach ($target in @("Argon2PoW-Challenge", "Argon2PoW-Solver")) {
        $package = Join-Path $distPath $target
        $env:QT_PLUGIN_PATH = $package
        $env:QT_QPA_PLATFORM_PLUGIN_PATH = Join-Path $package "platforms"
        foreach ($language in @("zh_CN", "en")) {
            $screenshot = Join-Path $shots "$target-$language.png"
            if (Test-Path -LiteralPath $screenshot) { Remove-Item -LiteralPath $screenshot }
            $arguments = @("--lang", $language, "--screenshot", ('"' + $screenshot + '"'),
                           "--screenshot-delay", "1200")
            if ($target -eq "Argon2PoW-Challenge") { $arguments += "--example" }
            else { $arguments += @("CHALLENGE:8192:1:SALT12345:1", "--auto-start") }
            $process = Start-Process -FilePath (Join-Path $package "$target.exe") `
                -ArgumentList $arguments -WindowStyle Hidden -PassThru
            if (-not $process.WaitForExit(20000)) {
                $process.Kill()
                throw "独立包启动超时：$target ($language)"
            }
            # Refresh the process handle before reading its exit code on Windows PowerShell.
            $process.Refresh()
            if ($process.ExitCode -ne 0 -or -not (Test-Path -LiteralPath $screenshot)) {
                throw "独立包启动失败：$target ($language)，退出码 $($process.ExitCode)"
            }
            Write-Host "通过：$target ($language)"
        }
    }
} finally {
    $env:Path = $originalPath
    $env:QT_PLUGIN_PATH = $originalPlugins
    $env:QT_QPA_PLATFORM = $originalPlatform
    $env:QT_QPA_PLATFORM_PLUGIN_PATH = $originalPlatformPlugins
}

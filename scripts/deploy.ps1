# SPDX-License-Identifier: GPL-3.0-only
[CmdletBinding()]
param(
    [string]$QtDir = "C:\Qt\6.12.0\mingw_64",
    [string]$MingwDir = "C:\Qt\Tools\mingw1310_64",
    [string]$BuildDir = "build",
    [string]$DistDir = "dist",
    [switch]$NoArchive
)

$ErrorActionPreference = "Stop"
$root = Split-Path $PSScriptRoot -Parent
$buildPath = [IO.Path]::GetFullPath((Join-Path $root $BuildDir))
$distPath = [IO.Path]::GetFullPath((Join-Path $root $DistDir))
$windeployqt = Join-Path $QtDir "bin\windeployqt.exe"
$strip = Join-Path $MingwDir "bin\strip.exe"
$env:Path = "$QtDir\bin;$MingwDir\bin;$env:Path"

foreach ($required in @($windeployqt, $strip,
    (Join-Path $QtDir "plugins\platforms\qwindows.dll"),
    (Join-Path $QtDir "translations\qtbase_zh_CN.qm"))) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) {
        throw "找不到打包依赖：$required"
    }
}

# Check every binary before replacing any existing package.
$targets = @("Argon2PoW-Challenge", "Argon2PoW-Solver")
foreach ($target in $targets) {
    if (-not (Test-Path -LiteralPath (Join-Path $buildPath "$target.exe") -PathType Leaf)) {
        throw "找不到 $target.exe，请先运行 build.ps1。"
    }
}
New-Item -ItemType Directory -Force -Path $distPath | Out-Null

function Assert-PackagePath([string]$PathToCheck) {
    $resolved = [IO.Path]::GetFullPath($PathToCheck)
    # Recursive cleanup is restricted to one named child of the distribution root.
    if ([IO.Path]::GetDirectoryName($resolved) -ne $distPath -or
        [IO.Path]::GetFileName($resolved) -notmatch '^(Argon2PoW-(Challenge|Solver)|\.stage-Argon2PoW-(Challenge|Solver)-[a-f0-9]+)$') {
        throw "拒绝清理打包目录之外的路径：$resolved"
    }
    if (Test-Path -LiteralPath $resolved) {
        $item = Get-Item -LiteralPath $resolved
        if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint) {
            throw "拒绝清理目录链接：$resolved"
        }
    }
}

$report = foreach ($target in $targets) {
    $output = Join-Path $distPath $target
    $staging = Join-Path $distPath ".stage-$target-$([Guid]::NewGuid().ToString('N'))"
    Assert-PackagePath $output
    Assert-PackagePath $staging
    New-Item -ItemType Directory -Path $staging | Out-Null
    try {
        $exe = Join-Path $staging "$target.exe"
        Copy-Item -LiteralPath (Join-Path $buildPath "$target.exe") -Destination $exe
        Copy-Item -LiteralPath (Join-Path $root "README.md") -Destination $staging
        Copy-Item -LiteralPath (Join-Path $root "LICENSE") -Destination $staging
        Copy-Item -LiteralPath (Join-Path $root "third_party\argon2\LICENSE") `
            -Destination (Join-Path $staging "LICENSE-argon2.txt")

        # Avoid plugin auto-scanning: it pulls in unused Network/Svg libraries.
        & $windeployqt --release --compiler-runtime --no-plugins --no-translations `
            --no-network --no-svg --no-opengl-sw --no-system-d3d-compiler `
            --dir $staging $exe | Out-Host
        if ($LASTEXITCODE -ne 0) { throw "windeployqt 失败：$target" }
        & $strip --strip-unneeded $exe
        if ($LASTEXITCODE -ne 0) { throw "strip 失败：$target" }

        foreach ($folder in @("platforms", "styles", "translations")) {
            New-Item -ItemType Directory -Path (Join-Path $staging $folder) | Out-Null
        }
        Copy-Item -LiteralPath (Join-Path $QtDir "plugins\platforms\qwindows.dll") `
            -Destination (Join-Path $staging "platforms")
        foreach ($style in @("qmodernwindowsstyle.dll", "qwindowsvistastyle.dll")) {
            $stylePath = Join-Path $QtDir "plugins\styles\$style"
            if (Test-Path -LiteralPath $stylePath) {
                Copy-Item -LiteralPath $stylePath -Destination (Join-Path $staging "styles")
            }
        }
        Copy-Item -LiteralPath (Join-Path $QtDir "translations\qtbase_zh_CN.qm") `
            -Destination (Join-Path $staging "translations")
        "[Paths]`nPlugins=.`nTranslations=translations" |
            Set-Content -LiteralPath (Join-Path $staging "qt.conf") -Encoding ascii

        # Replace the generated folder only after all dependencies were prepared.
        Assert-PackagePath $output
        if (Test-Path -LiteralPath $output) { Remove-Item -LiteralPath $output -Recurse -Force }
        Move-Item -LiteralPath $staging -Destination $output
        $archive = Join-Path $distPath "$target-windows-x64.zip"
        if (-not $NoArchive) {
            Compress-Archive -LiteralPath $output -DestinationPath $archive -CompressionLevel Optimal -Force
        }
        [PSCustomObject]@{
            Package = $target
            Bytes = (Get-ChildItem -LiteralPath $output -File -Recurse | Measure-Object Length -Sum).Sum
            ArchiveBytes = if (-not $NoArchive) { (Get-Item -LiteralPath $archive).Length } else { $null }
        }
    } finally {
        Assert-PackagePath $staging
        if (Test-Path -LiteralPath $staging) { Remove-Item -LiteralPath $staging -Recurse -Force }
    }
}
$report | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $distPath "package-sizes.json") -Encoding utf8
$report | Select-Object Package,
    @{n="Folder MiB";e={[math]::Round($_.Bytes / 1MB, 2)}},
    @{n="ZIP MiB";e={if ($null -ne $_.ArchiveBytes) {[math]::Round($_.ArchiveBytes / 1MB, 2)}}} |
    Format-Table -AutoSize

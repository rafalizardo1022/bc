param(
    [string]$Version = "",
    [ValidateSet("Release", "Debug")]
    [string]$Configuration = "Release",
    [ValidateSet("x64", "Win32")]
    [string]$Platform = "x64",
    [switch]$Run
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$project = Join-Path $repoRoot "src\Visual Studio solution\bridgecommand-mc.vcxproj"
$binDir = Join-Path $repoRoot "bin"
$baseExe = Join-Path $binDir "bridgecommand-mc.exe"

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (!(Test-Path $vswhere)) {
    throw "Could not find vswhere.exe. Install Visual Studio Build Tools or open the solution in Visual Studio and build bridgecommand-mc."
}

$msbuild = & $vswhere -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
if ([string]::IsNullOrWhiteSpace($msbuild)) {
    $msbuild = & $vswhere -latest -products * -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
}
if ([string]::IsNullOrWhiteSpace($msbuild)) {
    $knownMsBuildPaths = @(
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\amd64\MSBuild.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\amd64\MSBuild.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    )
    $msbuild = $knownMsBuildPaths | Where-Object { Test-Path $_ } | Select-Object -First 1
}
if ([string]::IsNullOrWhiteSpace($msbuild) -or !(Test-Path $msbuild)) {
    throw "Could not find MSBuild. Install Visual Studio Build Tools with C++ support."
}

Write-Host "Building bridgecommand-mc ($Configuration|$Platform)..."
& $msbuild $project /m "/p:Configuration=$Configuration" "/p:Platform=$Platform"

if (!(Test-Path $baseExe)) {
    throw "Build finished, but $baseExe was not found."
}

$date = Get-Date -Format "yyyyMMdd"
if ([string]::IsNullOrWhiteSpace($Version)) {
    $existing = Get-ChildItem -Path $binDir -Filter "bridgecommand-mc-$date-v*.exe" -ErrorAction SilentlyContinue
    $maxVersion = 0
    foreach ($file in $existing) {
        if ($file.BaseName -match "^bridgecommand-mc-$date-v(\d+)$") {
            $number = [int]$Matches[1]
            if ($number -gt $maxVersion) {
                $maxVersion = $number
            }
        }
    }
    $Version = "v$($maxVersion + 1)"
}

$versionedExe = Join-Path $binDir "bridgecommand-mc-$date-$Version.exe"
Copy-Item -LiteralPath $baseExe -Destination $versionedExe -Force

Write-Host ""
Write-Host "Created:"
Write-Host $versionedExe
Write-Host ""
Write-Host "Run it with:"
Write-Host "cd `"$binDir`""
Write-Host ".\$(Split-Path $versionedExe -Leaf)"

if ($Run) {
    Push-Location $binDir
    try {
        & ".\$(Split-Path $versionedExe -Leaf)"
    }
    finally {
        Pop-Location
    }
}

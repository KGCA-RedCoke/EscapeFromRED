param(
    [string]$CacheRoot = (Join-Path (Split-Path $PSScriptRoot -Parent) '..\EscapeFromRED-deps'),
    [string]$CMake,
    [string]$FbxSdkRoot = $env:FBXSDK_ROOT,
    [string]$FmodCoreRoot
)
$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$vendor = Join-Path $repo 'Engine\CoreEngine\Vender'
$CacheRoot = [IO.Path]::GetFullPath($CacheRoot)
if (!$CMake) {
    $cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
    if ($cmakeCommand) { $CMake = $cmakeCommand.Source }
    else {
        $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
        $vs = & $vswhere -latest -version '[17.0,18.0)' -products * -property installationPath
        $bundled = if ($vs) { Join-Path $vs 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' } else { '' }
        if ($bundled -and (Test-Path $bundled)) { $CMake = $bundled }
        elseif (Test-Path 'D:\UE\Engine\Extras\ThirdPartyNotUE\CMake\bin\cmake.exe') { $CMake = 'D:\UE\Engine\Extras\ThirdPartyNotUE\CMake\bin\cmake.exe' }
        else { throw 'Install CMake 3.20+ or pass -CMake with its executable path.' }
    }
}
if (!$FbxSdkRoot -and (Test-Path 'D:\UE\Engine\Source\ThirdParty\FBX\2020.2')) {
    $FbxSdkRoot = 'D:\UE\Engine\Source\ThirdParty\FBX\2020.2'
}
if (!$FbxSdkRoot -or !(Test-Path "$FbxSdkRoot\include\fbxsdk.h")) {
    throw 'Supply an installed Autodesk FBX SDK 2020.x via -FbxSdkRoot or FBXSDK_ROOT.'
}
$fbxLibRoot = @('vs2022','vs2019','vs2017') | ForEach-Object { Join-Path $FbxSdkRoot "lib\$_\x64" } | Where-Object { Test-Path "$_\release\libfbxsdk-md.lib" } | Select-Object -First 1
if (!$fbxLibRoot) { throw 'FBX SDK x64 /MD libraries not found.' }
function Run([string]$Program, [string[]]$Arguments) {
    & $Program @Arguments
    if ($LASTEXITCODE -ne 0) { throw "$Program failed ($LASTEXITCODE)" }
}
function Source([string]$Name, [string]$Url, [string]$Ref, [string]$Commit) {
    $path = Join-Path $CacheRoot $Name
    if (!(Test-Path $path)) { Run git @('clone','--depth','1','--branch',$Ref,$Url,$path) }
    $actual = & git -C $path rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $actual -ne $Commit) { throw "Unexpected source revision: $path" }
}
New-Item -ItemType Directory -Force $CacheRoot | Out-Null
Source 'fmt' 'https://github.com/fmtlib/fmt.git' '10.2.1' 'e69e5f977d458f2650bb346dadf2ad30c5320281'
Source 'spdlog' 'https://github.com/gabime/spdlog.git' 'v1.14.1' '27cb4c76708608465c413f6d0e6b8d99a4d84302'
Source 'DirectXTK' 'https://github.com/microsoft/DirectXTK.git' 'oct2024' '762657d34d0697cb9d6c71b74325e13f88a2658f'
$install = Join-Path $CacheRoot 'install'
Run $CMake @('-S',"$CacheRoot\fmt",'-B',"$CacheRoot\fmt-build",'-G','Visual Studio 17 2022','-A','x64','-DFMT_TEST=OFF','-DFMT_DOC=OFF',"-DCMAKE_INSTALL_PREFIX=$install")
foreach ($cfg in 'Debug','Release') { Run $CMake @('--build',"$CacheRoot\fmt-build",'--config',$cfg,'--target','install','--parallel','8') }
Run $CMake @('-S',"$CacheRoot\spdlog",'-B',"$CacheRoot\spdlog-build",'-G','Visual Studio 17 2022','-A','x64','-DSPDLOG_BUILD_EXAMPLE=OFF','-DSPDLOG_FMT_EXTERNAL=ON',"-DCMAKE_PREFIX_PATH=$install")
Run $CMake @('-S',"$CacheRoot\DirectXTK",'-B',"$CacheRoot\directxtk-build",'-G','Visual Studio 17 2022','-A','x64','-DBUILD_TOOLS=OFF','-DBUILD_XAUDIO_WIN8=OFF')
foreach ($cfg in 'Debug','Release') {
    Run $CMake @('--build',"$CacheRoot\spdlog-build",'--config',$cfg,'--parallel','8')
    Run $CMake @('--build',"$CacheRoot\directxtk-build",'--config',$cfg,'--parallel','8')
    $lib = Join-Path $vendor "lib\$($cfg.ToLower())"
    New-Item -ItemType Directory -Force $lib | Out-Null
    Copy-Item "$CacheRoot\fmt-build\$cfg\*.lib" $lib -Force
    Copy-Item "$CacheRoot\spdlog-build\$cfg\*.lib" $lib -Force
    Copy-Item "$CacheRoot\directxtk-build\lib\$cfg\DirectXTK.lib" $lib -Force
    foreach ($name in 'libfbxsdk-md.lib','libxml2-md.lib','zlib-md.lib') {
        Copy-Item "$fbxLibRoot\$($cfg.ToLower())\$name" $lib -Force
    }
}
New-Item -ItemType Directory -Force "$vendor\include\directxtk","$vendor\licenses" | Out-Null
Copy-Item "$CacheRoot\fmt\include\fmt" "$vendor\include" -Recurse -Force
Copy-Item "$CacheRoot\spdlog\include\spdlog" "$vendor\include" -Recurse -Force
Copy-Item "$CacheRoot\DirectXTK\Inc\*" "$vendor\include\directxtk" -Force
Copy-Item "$FbxSdkRoot\include\*" "$vendor\include" -Recurse -Force
foreach ($name in 'fmt','spdlog','DirectXTK') { Copy-Item "$CacheRoot\$name\LICENSE" "$vendor\licenses\$name-LICENSE" -Force }
if ($FmodCoreRoot) {
    # Supply an official FMOD Engine SDK api/core directory. Copy headers and binaries together.
    foreach ($file in 'inc\fmod.hpp','lib\x64\fmod_vc.lib','lib\x64\fmod.dll') {
        if (!(Test-Path (Join-Path $FmodCoreRoot $file))) { throw "Missing FMOD SDK file: $file" }
    }
    Copy-Item "$FmodCoreRoot\inc\fmod*" "$vendor\include" -Force
    foreach ($cfg in 'debug','release') {
        Copy-Item "$FmodCoreRoot\lib\x64\fmod_vc.lib" "$vendor\lib\$cfg" -Force
        Copy-Item "$FmodCoreRoot\lib\x64\fmod.dll" "$vendor\lib\$cfg" -Force
    }
} else {
    Write-Host 'Editor dependencies restored. Optional FMOD audio remains disabled.'
}

param(
    [ValidateSet('Debug','Release')][string]$Configuration = 'Debug',
    [switch]$Run,
    [switch]$SmokeTest
)
$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vs = & $vswhere -latest -version '[17.0,18.0)' -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (!$vs) { throw 'Visual Studio 2022 C++ tools are required.' }
$msbuild = Join-Path $vs 'MSBuild\Current\Bin\MSBuild.exe'
& $msbuild "$repo\Engine\Engine.sln" /m:8 "/p:Configuration=$Configuration" /p:Platform=x64 /nologo /v:minimal
if ($LASTEXITCODE -ne 0) { throw "Build failed ($LASTEXITCODE). Run Restore-Dependencies.ps1 first if SDK files are missing." }
if ($Run -or $SmokeTest) {
    $exe = Join-Path $repo "bin\$Configuration\Engine\CoreEngine\Output\CoreEngine.exe"
    $workingDirectory = Join-Path $repo 'Engine\CoreEngine'
    if ($SmokeTest) {
        $logs = Join-Path $repo "bin\Tests\$Configuration"
        New-Item -ItemType Directory -Force $logs | Out-Null
        $stdout = Join-Path $logs 'smoke.log'
        $stderr = Join-Path $logs 'smoke.err'
        $process = Start-Process $exe -ArgumentList '--smoke-test' -WorkingDirectory $workingDirectory -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
        if (!$process.WaitForExit(30000)) {
            Stop-Process -Id $process.Id
            throw "Editor smoke test timed out. See $logs"
        }
        if ($process.ExitCode -ne 0 -or !(Select-String -Path $stdout -Pattern 'Smoke test completed: 120 frames rendered.' -SimpleMatch -Quiet)) {
            throw "Editor smoke test failed (exit $($process.ExitCode)). See $logs"
        }
        Write-Host "$Configuration editor smoke test passed (120 frames, exit 0)."
    } else {
        # Interactive launch requested by -Run.
        Start-Process $exe -WorkingDirectory $workingDirectory
    }
}

# Dependencies and validation

## Pinned dependencies

| Dependency | Source/version | Result |
| --- | --- | --- |
| fmt | https://github.com/fmtlib/fmt/tree/10.2.1 | Debug/Release static libraries, dynamic CRT |
| spdlog | https://github.com/gabime/spdlog/tree/v1.14.1 | Debug/Release, external fmt |
| DirectXTK | https://github.com/microsoft/DirectXTK/tree/oct2024 | Debug/Release, optional XAudio/tools off |
| FBX SDK + libxml2 + zlib | Existing Autodesk FBX SDK 2020.2, x64 vs2017 libraries | Reused local SDK, verified with MSVC 14.38 |
| FMOD | Optional official Windows SDK: https://www.fmod.com/download#fmodengine | Disabled by default; not required for editor builds |

Versions are recovery candidates, not confirmed original versions. Public source commits are pinned in
`scripts/Restore-Dependencies.ps1`. The script builds both configurations and stages headers/libraries under
`Engine/CoreEngine/Vender`. Public dependency license notices are retained in `Vender/licenses`.
The source/build cache defaults to the sibling folder `EscapeFromRED-deps`. SDK binaries are not redistributed in Git.

## Restore options

The restore script accepts `-CMake`, `-CacheRoot`, `-FbxSdkRoot`, and `-FmodCoreRoot`.
CMake is discovered from PATH or Visual Studio. FBX uses `FBXSDK_ROOT` or an explicit SDK directory;
the recovery PC also has a fallback to its existing UE third-party SDK directory.
Supported FBX library layouts are `lib/{vs2022,vs2019,vs2017}/x64/{debug,release}`.

For optional audio, pass an official FMOD SDK `api/core` directory:

```powershell
.\scripts\Restore-Dependencies.ps1 -FmodCoreRoot 'C:\path\to\FMOD SDK\api\core'
```

This copies matching headers, `lib/x64/fmod_vc.lib`, and `lib/x64/fmod.dll` together.
Build with `/p:EnableFmod=true` to enable that backend; the DLL is copied beside the executable.
The enabled FMOD path is untested because the native SDK was unavailable during recovery.

## Validation on 2026-10-25

Environment: Windows x64, Visual Studio 2022, MSVC 14.38, Windows SDK 10.0.26100.0.

- Default editor build: Debug and Release compile and link without FMOD.
- `Build-Editor.ps1 -SmokeTest`: D3D11/ImGui startup, 120 render frames, success marker and exit 0.
- Separate dependency test: fmt formatting, spdlog logging, FBX manager/scene creation and
  DirectXTK Keyboard creation/reset passed in Debug and Release.
- The restore script was exercised on the local dependency cache.
- Startup fixes include zero/negative viewport dimensions during docking and an uninitialized FPS render target.

Scope: startup/render/shutdown only. Game assets, gameplay, FBX import and all editor interactions are not validated.
Missing original icons use generated white textures. Desktop screenshot verification was obscured by another
fullscreen application and is not counted as a visual pass.

Known pre-existing warnings include duplicate Node.cpp object output names and a local/temporary address
return in JSkeletalMeshObject.cpp. The older FBX SDK lacks debug PDBs, causing LNK4099 warnings.
These remain follow-up cleanup items; this recovery is not a full engine audit.

Reproducible runtime test logs: `bin/Tests/{Debug,Release}/smoke.log` and `smoke.err`.
Earlier recovery/build logs are retained locally in the sibling `EscapeFromRED-validation` folder.

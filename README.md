# EscapeFromRED / Jacob Engine

취업 준비 당시 제작했던 C++ / DirectX 11 엔진 포트폴리오입니다.
현재는 엔진을 공부하고 개선하기 위해 **에디터 단독 실행**을 기본 구성으로 복구했습니다.

## 시작하기

필요 환경: Windows x64, Visual Studio 2022의 C++ 데스크톱 개발 도구와 ATL,
Windows SDK, Git, CMake 3.20+, Autodesk FBX SDK 2020.x.

저장소 루트에서 PowerShell로 실행합니다.

```powershell
# 최초 1회: 공개 라이브러리를 고정 버전으로 빌드하고 설치된 FBX SDK를 연결합니다.
.\scripts\Restore-Dependencies.ps1 -FbxSdkRoot 'C:\path\to\FBX SDK\2020.x'

# 빌드 후 에디터 실행. 작업 디렉터리는 스크립트가 설정합니다.
.\scripts\Build-Editor.ps1 -Configuration Debug -Run

# 실제 D3D11 초기화, 120프레임 렌더링, 종료 코드 확인
.\scripts\Build-Editor.ps1 -Configuration Debug -SmokeTest
.\scripts\Build-Editor.ps1 -Configuration Release -SmokeTest
```

현재 복구한 PC에서는 기존 FBX SDK 경로가 자동 감지되므로 `-FbxSdkRoot`를 생략할 수 있습니다.
다른 PC에서는 직접 지정하거나 `FBXSDK_ROOT` 환경 변수를 설정하세요.

Visual Studio에서 `Engine/Engine.sln`을 열어 빌드할 수도 있습니다.
직접 실행/F5 실행 시 작업 디렉터리는 `Engine/CoreEngine`이어야 합니다.
출력 경로는 `bin/<Debug 또는 Release>/Engine/CoreEngine/Output/CoreEngine.exe`입니다.

## 현재 복구 범위

- FMOD 없이 에디터가 시작됩니다. 오디오는 기본 비활성화입니다.
- 게임 맵/HUD 대신 빈 씬과 뷰포트·인스펙터·에셋 브라우저를 초기화합니다.
- 누락된 아이콘/텍스처는 흰색 대체 텍스처로 표시됩니다.
- 레이아웃은 `Engine/CoreEngine/Saved/editor.ini`, 씬 저장은 `Saved/EditorLevel.jasset`을 사용합니다.
- 게임 Play는 비활성화했습니다. 기존 게임 에셋 전체, FBX 임포트, 개별 에디터 기능은 아직 검증하지 않았습니다.

기본값은 MSBuild 속성 `EngineEditorOnly=true`, `EnableFmod=false`입니다.
기존 게임 경로는 `EngineEditorOnly=false`로 남겨 두었지만, 누락된 리소스를 추가로 복구해야 합니다.
FMOD를 켜려면 동일 버전의 공식 헤더·라이브러리·DLL을 복구하고 `EnableFmod=true`로 빌드해야 합니다.

의존성 버전과 검증 내역은 [DEPENDENCIES.md](DEPENDENCIES.md)에 있습니다.
SDK, 빌드 결과, 로컬 로그와 에디터 저장 파일은 Git에 올리지 않습니다.

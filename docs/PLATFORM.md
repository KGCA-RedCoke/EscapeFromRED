# 플랫폼 분리 1단계: 창·입력·리소스 경로

이 단계는 기존 DX11 에디터의 Win32 코드를 별도 구현으로 옮기고,
동일한 창 인터페이스를 SDL3에서도 구현합니다. **전체 에디터/게임의 Linux·macOS 이식은 아직 아닙니다.**
CMake로 만드는 `PlatformSmoke`는 렌더러 없는 창입니다. 검은색/빈 창이 정상이며,
DX11 에디터의 `--smoke-test`와 달리 GPU 렌더링을 검증하지 않습니다.

## 무엇이 어디로 옮겨졌나

```text
기존 Application / 게임 입력
       │
       ├─ Core/Window/Window         기존 엔진용 어댑터, 주 창 1개
       ├─ Core/Input/KeyState       키 누름/유지/해제 전이 (OS 독립)
       │
       └─ Platform/IWindow          창·크기·입력 상태·이벤트·수명 계약
                  ├─ Win32          기존 Windows 에디터가 사용하는 구현
                  └─ SDL3           Linux/macOS 플랫폼 실행 파일이 사용하는 구현

DX11 swapchain ── Win32Handle()      HWND 변환은 Windows 연동 지점에만 존재
ImGui         ── ImGuiWindowWin32   메시지 연결; 플랫폼 라이브러리는 ImGui에 의존하지 않음
```

- `Platform/Window.h`와 기존 `Core/Window/Window.h`는 Windows/DirectX/ImGui 헤더 없이 컴파일됩니다.
- `WindowDesc::title`은 UTF-8이며 소유되는 문자열입니다. 임시 문자열 포인터를 보관하지 않습니다.
- 창 생성/이벤트/입력/파괴는 **메인 스레드**에서 호출합니다. SDL 구현은 주 창 1개를 지원하고 중복 생성을 거부합니다.
- `ClientSize`와 마우스 좌표는 창의 클라이언트 좌표입니다. `PixelSize` 및 resize callback은 렌더 타깃용 픽셀 크기입니다. Retina에서는 두 크기가 다를 수 있습니다.
- 현재 Win32 구현의 DPI 정책은 기존 에디터와 동일합니다. Per-monitor DPI 전환은 별도 작업입니다.
- `NativeHandle`은 빌린 핸들입니다. 호출자가 파괴하지 않습니다. 닫기는 요청 상태만 바꾸며 실제 파괴는 소유 객체의 소멸 시점입니다. 렌더러를 먼저 종료할 수 있습니다.
- `NativeEvent`의 포인터는 콜백 중에만 유효합니다. GUI가 이벤트를 소비해도 resize/close/입력 상태 갱신은 유지됩니다. bool 반환은 Win32 기본 메시지 처리의 소비 여부입니다.
- 키 이름은 엔진의 논리 키입니다. 기존 Q…Home 숫자 값은 유지하며 Enter를 추가했습니다. `End`는 기존 코드의 개수 센티널입니다.
- Win32는 현재 키 상태를 조회하고 SDL은 이벤트를 누적합니다. `KeyState`가 공통 Down/Pressed/Up/None 전이를 처리합니다. 한 프레임만 누른 키도 Up이 발생하고, 포커스 상실 시 한 번만 해제됩니다.
- 마우스 이동량은 클라이언트 좌표 차이입니다. 첫 프레임과 포커스 복귀 시 큰 이동량을 만들지 않습니다. Raw/relative mouse 입력은 후속 범위입니다.
- 마우스 제한은 창 클라이언트 영역을 사용하며 포커스 상실/종료 시 해제됩니다. 화면 절대 좌표는 Wayland 등에서 지원되지 않을 수 있으므로 게임 코드는 클라이언트 좌표를 사용합니다.

## 플랫폼 라이브러리 빌드

공통 필요 도구: C++20 컴파일러, CMake 3.24+, Git. Preset 사용 시 Ninja도 필요합니다.
Windows 전체 에디터에 필요한 FBX/ATL/fmt/spdlog는 이 작은 대상에는 필요하지 않습니다.
SDL은 `release-3.2.28`의 커밋 `7f3ae3d57459e59943a4ecfefc8f6277ec6bf540`을 고정해 CMake 빌드 디렉터리로 가져옵니다.
이미 설치한 SDL을 쓰려면 `-DJACOB_FETCH_SDL=OFF -DCMAKE_PREFIX_PATH=<SDL 설치 위치>`를 지정합니다.
기존 Windows 에디터에는 SDL 의존성이 추가되지 않습니다.

### Windows (Win32)

VS 2022 C++ 도구와 CMake가 설치된 터미널에서:

```powershell
cmake -S . -B out/build/platform -G "Visual Studio 17 2022" -A x64
cmake --build out/build/platform --config Debug --parallel 4
ctest --test-dir out/build/platform -C Debug --output-on-failure
.\out\build\platform\Debug\PlatformSmoke.exe
```

Release 검증은 위 명령의 `Debug`를 `Release`로 바꿉니다.
기존 에디터의 빌드/실행 명령은 그대로입니다:

```powershell
.\scripts\Build-Editor.ps1 -Configuration Debug -SmokeTest
.\scripts\Build-Editor.ps1 -Configuration Release -SmokeTest
.\scripts\Build-Editor.ps1 -Configuration Debug -Run
```

### Linux (SDL3 / X11)

Ubuntu/Debian 예시:

```sh
sudo apt-get install build-essential cmake ninja-build git libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxfixes-dev libxss-dev
cmake --preset platform -DSDL_WAYLAND=OFF
cmake --build --preset platform
ctest --preset platform
./out/build/platform/PlatformSmoke
```

디스플레이 없는 CI에서 실제 X11 이벤트 처리를 검증하려면 `xvfb`, `xauth`를 설치하고
`xvfb-run -a ctest --preset platform`을 사용합니다.
Wayland는 SDL 구현 경로가 있지만 이번 검증 대상은 X11입니다.

OS 창 없이 상태/이벤트 테스트만 할 때는:

```sh
cmake --preset platform-headless -DSDL_UNIX_CONSOLE_BUILD=ON -DSDL_X11=OFF -DSDL_WAYLAND=OFF
cmake --build --preset platform-headless
ctest --preset platform-headless
```

CTest가 창 테스트에만 `SDL_VIDEODRIVER=dummy`를 설정합니다. 이를 데스크톱/GPU 검증으로 해석하지 않습니다.

### macOS (SDL3 / Cocoa)

Xcode Command Line Tools, CMake, Ninja, Git 준비 후:

```sh
cmake --preset platform
cmake --build --preset platform
ctest --preset platform
./out/build/platform/PlatformSmoke
```

이 단계에는 Metal 렌더러가 없습니다. macOS 27 등 특정 OS/SDK 버전 호환성은 해당 Mac에서 따로 검증해야 합니다.
CI의 macOS 대상은 `macos-14`입니다.

## 리소스 경로

Windows 에디터 시작 시 `Platform::ResolveDataRoot`가 `Configs/engine.ini`와 `Shader`가 있는 디렉터리를 찾습니다.
우선순위는 명시적 `--data-root` → 현재 디렉터리 및 상위 저장소 → 실행 파일 디렉터리 및 상위 저장소입니다.
명시한 경로가 잘못되면 다른 경로로 조용히 대체하지 않고 오류를 반환합니다.

```powershell
& 'C:\projects\EscapeFromRED\bin\Debug\Engine\CoreEngine\Output\CoreEngine.exe' --data-root 'C:\projects\EscapeFromRED\Engine\CoreEngine'
```

Windows 인자는 `wmain`으로 받아 한글·공백 경로를 보존합니다.
기존 로더의 상대 경로를 유지하기 위해 **매니저/스레드 생성 전에 한 번만** 작업 디렉터리를 설정합니다.
모든 에셋 로더를 경로 서비스로 바꾸는 것은 후속 단계입니다. 외부 배포 시에는 리소스를 함께 배포하고 `--data-root`를 지정하세요.

## 테스트 범위

- `platform.paths`: 실행 위치 변화, 명시적 경로, 한글·공백, 누락 리소스 오류.
- `platform.input`: 짧은 탭, 유지 시간, 해제, 포커스 상실, 재입력, 잘못된 키.
- `platform.window`: 실제 백엔드 창 생성, resize 및 콜백, native event 전달, 종료와 재생성. SDL에서는 주입한 키/마우스 이벤트와 포커스 해제도 확인.
- `platform.facade`: 기존 엔진이 사용하는 어댑터의 문자열 소유, 초기화/종료, resize 상태 갱신.
- `platform.smoke`: 120회 이벤트 펌프 후 정상 종료. **120프레임 렌더링 테스트가 아닙니다.**

`.github/workflows/platform.yml`에서 Windows/Linux/macOS의 Debug/Release를 빌드·검증합니다.
워크플로 파일 추가는 실행 성공을 뜻하지 않으며, 실제 CI 결과와 아래 로컬 검증 기록을 구분합니다.

## 후속 범위

- 기존 Windows 에디터에서 resize/minimize/복귀, 도킹, 키보드·마우스를 재확인.
- SDL용 ImGui 플랫폼 연결과 그래픽 표면 생성. 현재 SDL 창은 그래픽 API 플래그 없이 생성합니다.
- D3D11 리소스/렌더링 인터페이스 분리 후 Metal·Vulkan 백엔드.
- `common_pch.h`의 Windows/ATL, DirectXMath/DirectWrite, FBX 로더는 아직 기존 엔진에 남아 있습니다.
- SDL의 다중 창, raw mouse, per-monitor DPI, 모든 에셋 경로의 UTF-8 규약은 별도 설계 대상입니다.

## 이번 변경의 로컬 검증 기록 (2026-10-25)

- Debian 13 / GCC 14: Xvfb X11 환경에서 Debug 및 Release 테스트 5개 통과.
- Debug의 자체 플랫폼 코드에 AddressSanitizer/UndefinedBehaviorSanitizer 적용 후 테스트 5개 통과.
- SDL dummy 드라이버의 헤드리스 테스트 통과. 데스크톱/GPU 검증과 구분합니다.
- MinGW-w64 GCC 14: Windows 대상 플랫폼 라이브러리, 테스트 실행 파일, 실제 ImGui Win32 연결 코드의 교차 컴파일 통과.
- Release 구성은 별도 빌드 디렉터리에서 SDL 소스를 다운로드하고 SHA-256을 검증하여 빌드했습니다.
- Windows MSVC 전체 에디터 실행 및 macOS Cocoa 실행은 이 Linux 클라우드에서 직접 검증하지 않았습니다.
  새 플랫폼 CI의 결과와 Windows `Build-Editor.ps1 -SmokeTest` 재실행으로 확인해야 합니다.

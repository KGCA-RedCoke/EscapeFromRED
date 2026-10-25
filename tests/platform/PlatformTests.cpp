#include "Platform/ResourcePaths.h"
#include "Platform/Window.h"
#include "Core/Window/Window.h"
#include "Core/Input/KeyState.h"

#ifdef JACOB_PLATFORM_SDL
#include <SDL3/SDL.h>
#else
#include "Platform/Win32/NativeWindow.h"
#endif

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string_view>
#include <thread>

namespace P = Jacob::Platform;
namespace fs = std::filesystem;
namespace
{
void Check(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}

template<class Function>
void Throws(Function operation, const char* message)
{
    bool threw = false;
    try { operation(); } catch (const std::exception&) { threw = true; }
    Check(threw, message);
}

struct TemporaryDirectory
{
    fs::path path;
    TemporaryDirectory()
    {
        std::random_device random;
        for (int attempt = 0; attempt < 100; ++attempt)
        {
            path = fs::temp_directory_path() / ("jacob-platform-" + std::to_string(random()));
            if (fs::create_directory(path)) return;
        }
        throw std::runtime_error("Cannot create temporary test directory.");
    }
    ~TemporaryDirectory() { std::error_code error; fs::remove_all(path, error); }
};

void Paths()
{
    TemporaryDirectory temp;
    const auto repo = temp.path / fs::path(u8"project with spaces-한글");
    const auto data = repo / "Engine" / "CoreEngine";
    fs::create_directories(data / "Configs");
    fs::create_directories(data / "Shader");
    std::ofstream(data / "Configs" / "engine.ini") << "[Editor.Window]\n";
    const auto executable = repo / "bin" / "Debug" / "Engine" / "CoreEngine" / "Output";
    fs::create_directories(executable);
    const auto unrelated = temp.path / "elsewhere";
    fs::create_directory(unrelated);
    const auto expected = fs::canonical(data);
    Check(P::ResolveDataRoot({}, data, {}) == expected, "Data-root working directory failed.");
    Check(P::ResolveDataRoot({}, repo, {}) == expected, "Repository-root launch failed.");
    Check(P::ResolveDataRoot({}, unrelated, executable) == expected, "Executable ancestor lookup failed.");
    Check(P::ResolveDataRoot("Engine/CoreEngine", repo, {}) == expected, "Relative override failed.");
    Check(P::ResolveDataRoot(data, unrelated, {}) == expected, "Absolute override failed.");
    Throws([&] { P::ResolveDataRoot(repo / "missing", repo, executable); }, "Invalid override silently fell back.");
    fs::remove(data / "Configs" / "engine.ini");
    Throws([&] { P::ResolveDataRoot({}, repo, executable); }, "Missing configuration was accepted.");
    Check(fs::is_directory(P::ExecutableDirectory()), "Executable directory does not exist.");
    std::cout << "Paths: cwd/repo/executable lookup, Unicode/spaces, explicit override and missing-resource errors passed.\n";
}

template<class Predicate>
void Wait(P::IWindow& window, Predicate predicate)
{
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    do
    {
        window.PollEvents();
        if (predicate()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    } while (std::chrono::steady_clock::now() < deadline);
    throw std::runtime_error("Timed out waiting for window event.");
}

#ifdef JACOB_PLATFORM_SDL
void Push(SDL_Event event)
{
    if (!SDL_PushEvent(&event)) throw std::runtime_error(SDL_GetError());
}
void Focus(SDL_WindowID id, bool focused)
{
    SDL_Event event{};
    event.type = focused ? SDL_EVENT_WINDOW_FOCUS_GAINED : SDL_EVENT_WINDOW_FOCUS_LOST;
    event.window.windowID = id;
    Push(event);
}
void Keyboard(SDL_WindowID id, bool down)
{
    SDL_Event event{};
    event.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    event.key.windowID = id;
    event.key.scancode = SDL_SCANCODE_Q; // Layout maps this physical key to logical W.
    event.key.key = SDLK_W;
    event.key.down = down;
    Push(event);
}
#endif

void WindowTest()
{
    P::WindowDesc desc;
    desc.title = "Jacob platform test - 한글";
    desc.hidden = true;
    desc.size = {320, 200};
    auto invalid = desc;
    invalid.size.width = 0;
    Throws([&] { P::CreatePlatformWindow(invalid); }, "Zero window size was accepted.");
    invalid.size.width = 0xffffffffu;
    Throws([&] { P::CreatePlatformWindow(invalid); }, "Overflowing window size was accepted.");
    auto window = P::CreatePlatformWindow(desc);
    Check(window->NativeHandle().value, "Window has no native handle.");
    Check(!window->IsClosed(), "New window is closed.");
    Check(window->ClientSize().width == 320 && window->ClientSize().height == 200, "Initial client size differs.");
    Check(window->PixelSize().width > 0 && window->PixelSize().height > 0, "Drawable extent is empty.");
    window->PollEvents();
    unsigned resizes = 0;
    P::Extent lastSize;
    window->SetResizeCallback([&](P::Extent size) { ++resizes; lastSize = size; });
    window->SetClientSize({480, 300});
    Wait(*window, [&] { return resizes && window->ClientSize().width == 480; });
    Check(lastSize.width == window->PixelSize().width && lastSize.height == window->PixelSize().height,
          "Resize callback must report drawable pixels.");
    Throws([&] { window->SetClientSize({0, 100}); }, "Zero resize was accepted.");
    Check(!window->ReadInput().IsDown(P::Key::End), "Key count sentinel was treated as a key.");

    // Exceptions from engine callbacks must never unwind through a native WndProc.
    window->SetResizeCallback([](P::Extent) { throw std::runtime_error("expected resize failure"); });
    bool callbackFailed = false;
    try
    {
        window->SetClientSize({500, 320});
        Wait(*window, [] { return false; });
    }
    catch (const std::runtime_error& error)
    {
        Check(std::string_view(error.what()) == "expected resize failure", "Resize callback exception was lost.");
        callbackFailed = true;
    }
    Check(callbackFailed, "Failing resize callback was not reported.");
    window->SetResizeCallback({});
    window->PollEvents(); // Finish the interrupted native event batch before injecting input.

    unsigned nativeEvents = 0;
    window->SetNativeEventFilter([&](const P::NativeEvent& event)
    {
        Check(event.window == window->NativeHandle().value, "Wrong native event window.");
        ++nativeEvents;
        return true; // GUI consumption must not suppress input/lifecycle bookkeeping.
    });
#ifdef JACOB_PLATFORM_SDL
    Throws([&] { P::CreatePlatformWindow(desc); }, "SDL accepted an unsupported second primary window.");
    auto* native = static_cast<SDL_Window*>(window->NativeHandle().value);
    const auto id = SDL_GetWindowID(native);
    Focus(id, true);
    Keyboard(id, true);
    Wait(*window, [&] { return window->ReadInput().IsDown(P::Key::W); });
    Keyboard(id, false);
    Wait(*window, [&] { return !window->ReadInput().IsDown(P::Key::W); });
    SDL_Event mouse{};
    mouse.type = SDL_EVENT_MOUSE_MOTION;
    mouse.motion.windowID = id;
    mouse.motion.x = 31;
    mouse.motion.y = 47;
    Push(mouse);
    mouse = {};
    mouse.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    mouse.button.windowID = id;
    mouse.button.button = SDL_BUTTON_LEFT;
    Push(mouse);
    Wait(*window, [&] { return window->ReadInput().IsDown(P::Key::LButton); });
    Check(window->ReadInput().clientPosition.x == 31 && window->ReadInput().clientPosition.y == 47,
          "Client mouse position was lost.");
    Keyboard(id, true);
    Focus(id, false);
    Wait(*window, [&] { return !window->ReadInput().focused; });
    Check(!window->ReadInput().IsDown(P::Key::W), "Focus loss left keys held.");
    Focus(id, true);
    Wait(*window, [&] { return window->ReadInput().focused; });
    Check(!window->ReadInput().IsDown(P::Key::W) && !window->ReadInput().IsDown(P::Key::LButton),
          "Focus regain resurrected held keys.");
    SDL_Event close{};
    close.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
    close.window.windowID = id;
    Push(close);
#else
    const auto native = P::Win32Handle(window->NativeHandle());
    Check(GetWindowTextLengthW(native) > 0, "UTF-8 title was not installed.");
    PostMessageW(native, WM_MOUSEMOVE, 0, 0);
    PostMessageW(native, WM_CLOSE, 0, 0);
#endif
    Wait(*window, [&] { return window->IsClosed(); });
    Check(nativeEvents > 0, "Native event filter was never called.");
    // The renderer may still own resources: close is a request, destruction is RAII.
    Check(window->NativeHandle().value, "Close destroyed the native handle prematurely.");
    window.reset();
    auto recreated = P::CreatePlatformWindow(desc);
    Check(!recreated->IsClosed(), "Window recreation inherited close state.");
    recreated->RequestClose();
    Check(recreated->IsClosed(), "Programmatic close failed.");
    std::cout << "Window: creation, resize, native event routing, close and recreation passed.\n";
#ifdef JACOB_PLATFORM_SDL
    std::cout << "SDL event injection: keyboard/mouse transitions and focus-loss clearing passed (not physical input).\n";
#endif
}

void Input()
{
    using Jacob::Input::KeyPhase;
    Jacob::Input::KeyState key{P::Key::W};
    const auto start = std::chrono::steady_clock::time_point{};
    key.Update(true, start);
    Check(key.State == KeyPhase::Down, "Initial press did not emit Down.");
    key.Update(false, start + std::chrono::milliseconds(1));
    Check(key.State == KeyPhase::Up, "One-frame tap did not emit Up.");
    key.Update(false, start + std::chrono::milliseconds(2));
    Check(key.State == KeyPhase::None, "Release was emitted twice.");
    key.Update(true, start + std::chrono::milliseconds(10));
    key.Update(true, start + std::chrono::milliseconds(35));
    Check(key.State == KeyPhase::Pressed && key.PressDuration.count() == 25, "Held-key duration is incorrect.");
    key.Update(false, start + std::chrono::milliseconds(40)); // Also used for focus loss.
    Check(key.State == KeyPhase::Up && key.PressDuration.count() == 0, "Focus-loss release did not reset duration.");
    key.Update(true, start + std::chrono::milliseconds(50));
    Check(key.State == KeyPhase::Down && key.PressDuration.count() == 0, "Re-press inherited the old hold.");
    P::InputSnapshot snapshot;
    snapshot.keys[static_cast<std::size_t>(P::Key::W)] = true;
    Check(!snapshot.IsDown(P::Key::W), "Unfocused snapshot exposed a pressed key.");
    snapshot.focused = true;
    Check(snapshot.IsDown(P::Key::W), "Focused snapshot lost a pressed key.");
    Check(!snapshot.IsDown(static_cast<P::Key>(255)), "Invalid key index was accepted.");
    std::cout << "Input: tap, hold duration, release, focus loss, re-press and invalid keys passed.\n";
}

void Facade()
{
    Throws([] { Window::GetWindow(); }, "Facade allowed access before initialization.");
    for (int i = 0; i < 2; ++i)
    {
        Window window(std::string("Owned temporary title").c_str(), {320, 200, false, true});
        Check(std::string_view(window.GetWindowTitle()) == "Owned temporary title", "Facade did not own its title.");
        window.Initialize();
        Check(Window::GetWindow() == &window, "Primary facade registration failed.");
        Check(window.GetNativeHandle().value, "Facade has no handle.");
        Throws([&] { window.Initialize(); }, "Facade allowed duplicate initialization.");
        Throws([] { Window second; second.Initialize(); }, "Facade accepted two primary windows.");
        Check(Window::GetWindow() == &window, "Rejected window destroyed the primary registration.");
        unsigned resizes = 0;
        window.ResizeCallbacks.push_back([&](std::uint32_t width, std::uint32_t height)
        {
            Check(window.GetWindowWidth() == width && window.GetWindowHeight() == height,
                  "Facade size was stale inside callback.");
            ++resizes;
        });
        window.OnResize(400, 250);
        Check(resizes == 1, "Facade resize was not forwarded.");
        window.Update();
    }
    Throws([] { Window::GetWindow(); }, "Facade left a dangling primary window.");
    std::cout << "Facade: owned title, registration, resize ordering, duplicate-init rejection and cleanup passed.\n";
}
}

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2) throw std::runtime_error("Specify paths, input, window or facade.");
        const std::string_view suite = argv[1];
        if (suite == "paths") Paths();
        else if (suite == "input") Input();
        else if (suite == "window") WindowTest();
        else if (suite == "facade") Facade();
        else throw std::runtime_error("Unknown test suite.");
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

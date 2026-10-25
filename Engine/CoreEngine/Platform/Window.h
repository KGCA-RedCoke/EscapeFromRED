#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>

namespace Jacob::Platform
{
// Keep existing engine key values stable. End is the count sentinel, not the End key.
enum class Key : std::uint8_t
{
    Q, W, E, R, T, Y, U, I, O, P,
    A, S, D, F, G, H, J, K, L,
    Z, X, C, V, B, N, M,
    Left, Right, Down, Up, LButton, MButton, RButton,
    Space, F1, F2, F3, F4, LShift, Home, Enter, End
};

struct Point { int x = 0; int y = 0; };
struct Extent { std::uint32_t width = 0; std::uint32_t height = 0; };

struct InputSnapshot
{
    bool focused = false;
    Point screenPosition;
    Point clientPosition;
    std::array<bool, static_cast<std::size_t>(Key::End)> keys{};

    bool IsDown(Key key) const noexcept
    {
        const auto index = static_cast<std::size_t>(key);
        return focused && index < keys.size() && keys[index];
    }
};

enum class NativeWindowKind { Win32, SDL };
struct NativeWindowHandle
{
    NativeWindowKind kind;
    void* value = nullptr; // Borrowed; valid only while the window is alive.
};

// Escape hatch for GUI integrations. SDL passes a borrowed SDL_Event in payload;
// Win32 uses message/wparam/lparam. Never retain payload after the callback returns.
struct NativeEvent
{
    NativeWindowKind kind;
    void* window = nullptr;
    const void* payload = nullptr;
    std::uint32_t message = 0;
    std::uintptr_t wparam = 0;
    std::intptr_t lparam = 0;
};

struct WindowDesc
{
    std::string title = "Jacob Engine"; // UTF-8, owned by the backend.
    Extent size{1280, 720}; // Client coordinates; PixelSize is for render targets.
    bool hidden = false;
    bool fullscreen = false;
    std::filesystem::path iconPath;
};

// All methods, creation and destruction run on the main thread. The current
// engine owns one primary window. Graphics/ImGui are deliberately not dependencies.
class IWindow
{
public:
    virtual ~IWindow() = default;
    virtual void PollEvents() = 0;
    virtual bool IsClosed() const noexcept = 0;
    virtual void RequestClose() noexcept = 0;
    virtual Extent ClientSize() const noexcept = 0;
    virtual Extent PixelSize() const noexcept = 0;
    virtual void SetClientSize(Extent size) = 0;
    virtual InputSnapshot ReadInput() const = 0;
    virtual void SetMouseCaptured(bool captured) = 0;
    virtual void WarpMouse(Point clientPosition) = 0;
    virtual NativeWindowHandle NativeHandle() const noexcept = 0;
    // Drawable pixels, including {0,0} when minimized. Renderers must skip zero-size targets.
    virtual void SetResizeCallback(std::function<void(Extent)> callback) = 0;
    virtual void SetNativeEventFilter(std::function<bool(const NativeEvent&)> filter) = 0;
};

// Throws std::runtime_error on initialization errors, std::invalid_argument for
// invalid dimensions. Exactly one implementation is linked into each target.
std::unique_ptr<IWindow> CreatePlatformWindow(const WindowDesc& desc);
std::filesystem::path ExecutableDirectory();
}

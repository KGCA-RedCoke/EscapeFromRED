#pragma once
#include "Platform/Window.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

struct FBasicWindowData
{
    std::uint32_t Width;
    std::uint32_t Height;
    bool bFullScreen;
    bool bVsync;
};

// Compatibility facade for the existing engine. The platform backend owns the
// OS window; graphics and GUI only borrow the tagged native handle.
class Window
{
public:
    Window();
    Window(const char* title, const FBasicWindowData& data);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void Initialize();
    void Update();
    static Window* GetWindow();

    Jacob::Platform::NativeWindowHandle GetNativeHandle() const;
    const char* GetWindowTitle() const { return mTitle.c_str(); }
    const FBasicWindowData& GetWindowData() const { return mWindowData; }
    std::uint32_t GetWindowWidth() const { return mWindowData.Width; }
    std::uint32_t GetWindowHeight() const { return mWindowData.Height; }
    bool IsClosed() const;
    bool IsFullScreen() const { return mWindowData.bFullScreen; }
    bool IsVsyncEnabled() const { return mWindowData.bVsync; }

    void OnResize(std::uint32_t width, std::uint32_t height);
    void LockMouseToWindow() const;
    void WarpMouse(Jacob::Platform::Point position) const;
    Jacob::Platform::InputSnapshot ReadInput() const;
    void SetNativeEventFilter(std::function<bool(const Jacob::Platform::NativeEvent&)> filter);

    std::vector<std::function<void(std::uint32_t, std::uint32_t)>> ResizeCallbacks;

private:
    std::string mTitle;
    FBasicWindowData mWindowData;
    std::unique_ptr<Jacob::Platform::IWindow> mBackend;
    static Window* s_Window;
};

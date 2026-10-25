#include "Window.h"

#include <stdexcept>
#include <utility>

Window* Window::s_Window = nullptr;

Window::Window() : Window("Jacob Engine", {1280, 800, false, false}) {}

Window::Window(const char* title, const FBasicWindowData& data)
    : mTitle(title ? title : "Jacob Engine"), mWindowData(data) {}

Window::~Window()
{
    if (s_Window == this) s_Window = nullptr;
}

void Window::Initialize()
{
    if (mBackend || s_Window) throw std::logic_error("The engine already has a primary window.");
    Jacob::Platform::WindowDesc desc;
    desc.title = mTitle;
    desc.size = {mWindowData.Width, mWindowData.Height};
    desc.fullscreen = mWindowData.bFullScreen;
    desc.iconPath = "jacobengine.ico";
    mBackend = Jacob::Platform::CreatePlatformWindow(desc);
    mBackend->SetResizeCallback([this](Jacob::Platform::Extent size) { OnResize(size.width, size.height); });
    const auto size = mBackend->PixelSize();
    mWindowData.Width = size.width;
    mWindowData.Height = size.height;
    s_Window = this;
}

void Window::Update() { mBackend->PollEvents(); }
bool Window::IsClosed() const { return mBackend && mBackend->IsClosed(); }
Jacob::Platform::NativeWindowHandle Window::GetNativeHandle() const { return mBackend->NativeHandle(); }
Jacob::Platform::InputSnapshot Window::ReadInput() const { return mBackend->ReadInput(); }
void Window::LockMouseToWindow() const { mBackend->SetMouseCaptured(true); }
void Window::WarpMouse(Jacob::Platform::Point position) const { mBackend->WarpMouse(position); }
void Window::SetNativeEventFilter(std::function<bool(const Jacob::Platform::NativeEvent&)> filter)
{
    mBackend->SetNativeEventFilter(std::move(filter));
}

void Window::OnResize(std::uint32_t width, std::uint32_t height)
{
    mWindowData.Width = width;
    mWindowData.Height = height;
    for (const auto& callback : ResizeCallbacks) callback(width, height);
}

Window* Window::GetWindow()
{
    if (!s_Window) throw std::logic_error("The primary window has not been initialized.");
    return s_Window;
}

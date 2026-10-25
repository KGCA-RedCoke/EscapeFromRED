#include <exception>
#include <limits>
#include <string_view>
#include <utility>
#include <vector>
#include "Platform/Win32/NativeWindow.h"

namespace Jacob::Platform
{
namespace
{
constexpr wchar_t WindowClassName[] = L"Jacob.Platform.Window";

std::wstring WideTitle(std::string_view text)
{
    if (text.empty()) return {};
    if (text.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::invalid_argument("Window title is too long.");
    const auto length = static_cast<int>(text.size());
    const int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), length, nullptr, 0);
    if (!count) throw std::invalid_argument("Window title must be valid UTF-8.");
    std::wstring result(count, L'\0');
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), length, result.data(), count))
        throw std::runtime_error("Window title conversion failed.");
    return result;
}

constexpr int VirtualKeys[] = {
    'Q','W','E','R','T','Y','U','I','O','P',
    'A','S','D','F','G','H','J','K','L','Z','X','C','V','B','N','M',
    VK_LEFT,VK_RIGHT,VK_DOWN,VK_UP,VK_LBUTTON,VK_MBUTTON,VK_RBUTTON,
    VK_SPACE,VK_F1,VK_F2,VK_F3,VK_F4,VK_LSHIFT,VK_HOME,VK_RETURN
};
static_assert(std::size(VirtualKeys) == static_cast<std::size_t>(Key::End));

void ValidateSize(Extent size)
{
    // Leave room for non-client decorations in signed Win32 coordinates.
    constexpr auto limit = static_cast<std::uint32_t>((std::numeric_limits<int>::max)() / 2);
    if (!size.width || !size.height || size.width > limit || size.height > limit)
        throw std::invalid_argument("Window dimensions must be positive and fit native coordinates.");
}

class Win32Window final : public IWindow
{
public:
    explicit Win32Window(const WindowDesc& desc)
    {
        ValidateSize(desc.size);
        const auto title = WideTitle(desc.title);
        const auto instance = GetModuleHandleW(nullptr);
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.hInstance = instance;
        windowClass.lpfnWndProc = Procedure;
        windowClass.lpszClassName = WindowClassName;
        windowClass.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
        windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        if (!RegisterClassExW(&windowClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            throw std::runtime_error("RegisterClassExW failed.");

        const DWORD style = desc.fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
        RECT rect{0, 0, static_cast<LONG>(desc.size.width), static_cast<LONG>(desc.size.height)};
        if (!AdjustWindowRectEx(&rect, style, FALSE, WS_EX_APPWINDOW))
            throw std::runtime_error("AdjustWindowRectEx failed.");
        const int width = desc.fullscreen ? GetSystemMetrics(SM_CXSCREEN) : rect.right - rect.left;
        const int height = desc.fullscreen ? GetSystemMetrics(SM_CYSCREEN) : rect.bottom - rect.top;
        mHandle = CreateWindowExW(WS_EX_APPWINDOW, WindowClassName, title.c_str(),
                                 style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                 (GetSystemMetrics(SM_CXSCREEN) - width) / 2,
                                 (GetSystemMetrics(SM_CYSCREEN) - height) / 2,
                                 width, height, nullptr, nullptr, instance, this);
        if (!mHandle) throw std::runtime_error("CreateWindowExW failed.");
        RECT client{};
        GetClientRect(mHandle, &client);
        mSize = {static_cast<std::uint32_t>(client.right), static_cast<std::uint32_t>(client.bottom)};
        if (!desc.iconPath.empty())
        {
            mIcon = static_cast<HICON>(LoadImageW(nullptr, desc.iconPath.c_str(), IMAGE_ICON,
                                                 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
            if (mIcon) SendMessageW(mHandle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(mIcon));
        }
        if (!desc.hidden)
        {
            ShowWindow(mHandle, SW_SHOW);
            SetFocus(mHandle);
        }
    }

    ~Win32Window() override
    {
        ReleaseCaptureState();
        // Callbacks may reference GUI objects already destroyed by the application.
        mResize = {};
        mFilter = {};
        if (mHandle) DestroyWindow(mHandle);
        if (mIcon) DestroyIcon(mIcon);
    }

    void PollEvents() override
    {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT) { mClosed = true; continue; }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        if (mCallbackError)
        {
            auto error = std::exchange(mCallbackError, {});
            std::rethrow_exception(error);
        }
    }

    bool IsClosed() const noexcept override { return mClosed; }
    void RequestClose() noexcept override { mClosed = true; ReleaseCaptureState(); }
    Extent ClientSize() const noexcept override { return mSize; }
    Extent PixelSize() const noexcept override { return mSize; }
    NativeWindowHandle NativeHandle() const noexcept override { return {NativeWindowKind::Win32, mHandle}; }
    void SetResizeCallback(std::function<void(Extent)> callback) override { mResize = std::move(callback); }
    void SetNativeEventFilter(std::function<bool(const NativeEvent&)> filter) override { mFilter = std::move(filter); }

    void SetClientSize(Extent size) override
    {
        ValidateSize(size);
        RECT rect{0, 0, static_cast<LONG>(size.width), static_cast<LONG>(size.height)};
        if (!AdjustWindowRectEx(&rect, static_cast<DWORD>(GetWindowLongPtrW(mHandle, GWL_STYLE)), FALSE,
                               static_cast<DWORD>(GetWindowLongPtrW(mHandle, GWL_EXSTYLE))) ||
            !SetWindowPos(mHandle, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                          SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE))
            throw std::runtime_error("Window resize failed.");
    }

    InputSnapshot ReadInput() const override
    {
        InputSnapshot input;
        const auto focus = GetFocus();
        input.focused = focus == mHandle || (focus && IsChild(mHandle, focus));
        POINT position{};
        if (GetCursorPos(&position))
        {
            input.screenPosition = {position.x, position.y};
            if (ScreenToClient(mHandle, &position)) input.clientPosition = {position.x, position.y};
        }
        if (input.focused)
            for (std::size_t i = 0; i < input.keys.size(); ++i)
                input.keys[i] = (GetAsyncKeyState(VirtualKeys[i]) & 0x8000) != 0;
        return input;
    }

    void SetMouseCaptured(bool captured) override
    {
        if (!captured) { ReleaseCaptureState(); return; }
        if (mCaptured) return;
        SetForegroundWindow(mHandle);
        SetFocus(mHandle);
        if (!ClipToClient()) throw std::runtime_error("Mouse capture failed.");
        // Balance every change to the per-thread cursor display counter.
        do { ++mHideCalls; } while (ShowCursor(FALSE) >= 0);
        mCaptured = true;
    }

    void WarpMouse(Point position) override
    {
        POINT point{position.x, position.y};
        if (!ClientToScreen(mHandle, &point) || !SetCursorPos(point.x, point.y))
            throw std::runtime_error("Mouse positioning failed.");
    }

private:
    bool ClipToClient() const
    {
        RECT rect{};
        if (!GetClientRect(mHandle, &rect)) return false;
        POINT topLeft{rect.left, rect.top}, bottomRight{rect.right, rect.bottom};
        if (!ClientToScreen(mHandle, &topLeft) || !ClientToScreen(mHandle, &bottomRight)) return false;
        rect = {topLeft.x, topLeft.y, bottomRight.x, bottomRight.y};
        return ClipCursor(&rect) != FALSE;
    }

    void ReleaseCaptureState() noexcept
    {
        if (mCaptured) ClipCursor(nullptr);
        while (mHideCalls > 0) { ShowCursor(TRUE); --mHideCalls; }
        mCaptured = false;
    }

    static LRESULT CALLBACK Procedure(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
    {
        auto* self = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(handle, GWLP_USERDATA));
        if (message == WM_NCCREATE)
        {
            self = static_cast<Win32Window*>(reinterpret_cast<CREATESTRUCTW*>(lparam)->lpCreateParams);
            self->mHandle = handle;
            SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        if (!self) return DefWindowProcW(handle, message, wparam, lparam);
        try
        {
            if (message == WM_CLOSE) { self->RequestClose(); return 0; }
            if (message == WM_SIZE)
            {
                self->mSize = {LOWORD(lparam), HIWORD(lparam)};
                if (self->mResize) self->mResize(self->mSize);
            }
            if (message == WM_KILLFOCUS) self->ReleaseCaptureState();
            if ((message == WM_MOVE || message == WM_SIZE) && self->mCaptured) self->ClipToClient();
            if (message == WM_DESTROY) self->mClosed = true;
            if (message == WM_NCDESTROY)
            {
                SetWindowLongPtrW(handle, GWLP_USERDATA, 0);
                self->mHandle = nullptr;
            }
            if (self->mFilter && self->mFilter({NativeWindowKind::Win32, handle, nullptr, message, wparam, lparam}))
                return 1;
        }
        catch (...) { self->mCallbackError = std::current_exception(); }
        return DefWindowProcW(handle, message, wparam, lparam);
    }

    HWND mHandle = nullptr;
    HICON mIcon = nullptr;
    Extent mSize;
    bool mClosed = false;
    bool mCaptured = false;
    int mHideCalls = 0;
    std::function<void(Extent)> mResize;
    std::function<bool(const NativeEvent&)> mFilter;
    std::exception_ptr mCallbackError;
};
}

std::unique_ptr<IWindow> CreatePlatformWindow(const WindowDesc& desc) { return std::make_unique<Win32Window>(desc); }

std::filesystem::path ExecutableDirectory()
{
    std::vector<wchar_t> path(256);
    for (;;)
    {
        const auto size = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
        if (!size) throw std::runtime_error("GetModuleFileNameW failed.");
        if (size < path.size()) return std::filesystem::path(std::wstring(path.data(), size)).parent_path();
        path.resize(path.size() * 2);
    }
}
}

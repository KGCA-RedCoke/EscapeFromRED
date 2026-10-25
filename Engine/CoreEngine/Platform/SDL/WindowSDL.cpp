#include "Platform/Window.h"

#include <SDL3/SDL.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>
#include <limits>
#include <stdexcept>
#include <utility>

namespace Jacob::Platform
{
namespace
{
// SDL's event queue is process-wide. This first implementation deliberately
// supports one primary window; a future multi-window dispatcher must own polling.
bool WindowAlive = false;

constexpr SDL_Keycode KeyCodes[] = {
    SDLK_Q,SDLK_W,SDLK_E,SDLK_R,SDLK_T,
    SDLK_Y,SDLK_U,SDLK_I,SDLK_O,SDLK_P,
    SDLK_A,SDLK_S,SDLK_D,SDLK_F,SDLK_G,
    SDLK_H,SDLK_J,SDLK_K,SDLK_L,
    SDLK_Z,SDLK_X,SDLK_C,SDLK_V,SDLK_B,SDLK_N,SDLK_M,
    SDLK_LEFT,SDLK_RIGHT,SDLK_DOWN,SDLK_UP,
    SDLK_UNKNOWN,SDLK_UNKNOWN,SDLK_UNKNOWN,
    SDLK_SPACE,SDLK_F1,SDLK_F2,SDLK_F3,SDLK_F4,
    SDLK_LSHIFT,SDLK_HOME,SDLK_RETURN
};
static_assert(std::size(KeyCodes) == static_cast<std::size_t>(Key::End));

void ValidateSize(Extent size)
{
    constexpr auto limit = static_cast<std::uint32_t>(std::numeric_limits<int>::max() / 2);
    if (!size.width || !size.height || size.width > limit || size.height > limit)
        throw std::invalid_argument("Window dimensions must be positive and fit native coordinates.");
}

std::runtime_error Error(const char* operation)
{
    return std::runtime_error(std::string(operation) + ": " + SDL_GetError());
}

class SDLWindow final : public IWindow
{
public:
    explicit SDLWindow(const WindowDesc& desc)
    {
        ValidateSize(desc.size);
        if (WindowAlive) throw std::logic_error("SDL platform currently supports one primary window.");
        // The engine owns main(); SDL must not replace the application's entry point.
        SDL_SetMainReady();
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) throw Error("SDL video initialization");
        SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        if (desc.hidden) flags |= SDL_WINDOW_HIDDEN;
        if (desc.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
        mWindow = SDL_CreateWindow(desc.title.c_str(), static_cast<int>(desc.size.width),
                                  static_cast<int>(desc.size.height), flags);
        if (!mWindow)
        {
            auto error = Error("SDL window creation");
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            throw error;
        }
        mInput.focused = (SDL_GetWindowFlags(mWindow) & SDL_WINDOW_INPUT_FOCUS) != 0;
        WindowAlive = true;
    }

    ~SDLWindow() override
    {
        ReleaseCapture();
        SDL_DestroyWindow(mWindow);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        WindowAlive = false;
    }

    void PollEvents() override
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT) RequestClose();
            if (event.type != SDL_EVENT_QUIT && SDL_GetWindowFromEvent(&event) != mWindow) continue;
            // Lifecycle/input state must advance even when a GUI consumes an event.
            switch (event.type)
            {
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: RequestClose(); break;
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                if (mResize) mResize(PixelSize());
                break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED: mInput.focused = true; break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                mInput.focused = false;
                mInput.keys.fill(false);
                ReleaseCapture();
                break;
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
            {
                // Match Win32 virtual-key semantics rather than US physical scancodes.
                auto keycode = event.key.key;
                if (keycode >= 'A' && keycode <= 'Z') keycode += 'a' - 'A';
                for (std::size_t i = 0; i < std::size(KeyCodes); ++i)
                    if (KeyCodes[i] != SDLK_UNKNOWN && KeyCodes[i] == keycode)
                        mInput.keys[i] = event.type == SDL_EVENT_KEY_DOWN;
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                Key key = Key::End;
                if (event.button.button == SDL_BUTTON_LEFT) key = Key::LButton;
                if (event.button.button == SDL_BUTTON_MIDDLE) key = Key::MButton;
                if (event.button.button == SDL_BUTTON_RIGHT) key = Key::RButton;
                if (key != Key::End) mInput.keys[static_cast<std::size_t>(key)] = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
                break;
            }
            case SDL_EVENT_MOUSE_MOTION:
                mInput.clientPosition = {static_cast<int>(event.motion.x), static_cast<int>(event.motion.y)};
                break;
            }
            if (mFilter) mFilter({NativeWindowKind::SDL, mWindow, &event});
        }
    }

    bool IsClosed() const noexcept override { return mClosed; }
    void RequestClose() noexcept override { mClosed = true; ReleaseCapture(); }
    Extent ClientSize() const noexcept override
    {
        int width = 0, height = 0;
        SDL_GetWindowSize(mWindow, &width, &height);
        return {static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)};
    }
    Extent PixelSize() const noexcept override
    {
        int width = 0, height = 0;
        SDL_GetWindowSizeInPixels(mWindow, &width, &height);
        return {static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)};
    }
    void SetClientSize(Extent size) override
    {
        ValidateSize(size);
        if (!SDL_SetWindowSize(mWindow, static_cast<int>(size.width), static_cast<int>(size.height)))
            throw Error("SDL window resize");
    }
    InputSnapshot ReadInput() const override
    {
        auto result = mInput;
        float x = 0, y = 0;
        SDL_GetGlobalMouseState(&x, &y); // May be unavailable (e.g. Wayland); clientPosition remains usable.
        result.screenPosition = {static_cast<int>(x), static_cast<int>(y)};
        return result;
    }
    void SetMouseCaptured(bool captured) override
    {
        if (!captured) { ReleaseCapture(); return; }
        if (mCaptured) return;
        if (!SDL_SetWindowMouseGrab(mWindow, true)) throw Error("SDL mouse grab");
        mCursorWasVisible = SDL_CursorVisible();
        if (!SDL_HideCursor())
        {
            SDL_SetWindowMouseGrab(mWindow, false);
            throw Error("SDL hide cursor");
        }
        mCaptured = true;
    }
    void WarpMouse(Point position) override { SDL_WarpMouseInWindow(mWindow, static_cast<float>(position.x), static_cast<float>(position.y)); }
    NativeWindowHandle NativeHandle() const noexcept override { return {NativeWindowKind::SDL, mWindow}; }
    void SetResizeCallback(std::function<void(Extent)> callback) override { mResize = std::move(callback); }
    void SetNativeEventFilter(std::function<bool(const NativeEvent&)> filter) override { mFilter = std::move(filter); }

private:
    void ReleaseCapture() noexcept
    {
        if (!mCaptured) return;
        SDL_SetWindowMouseGrab(mWindow, false);
        if (mCursorWasVisible) SDL_ShowCursor();
        mCaptured = false;
    }

    SDL_Window* mWindow = nullptr;
    InputSnapshot mInput;
    bool mClosed = false;
    bool mCaptured = false;
    bool mCursorWasVisible = true;
    std::function<void(Extent)> mResize;
    std::function<bool(const NativeEvent&)> mFilter;
};
}

std::unique_ptr<IWindow> CreatePlatformWindow(const WindowDesc& desc) { return std::make_unique<SDLWindow>(desc); }

std::filesystem::path ExecutableDirectory()
{
    const char* path = SDL_GetBasePath();
    if (!path) throw Error("SDL executable directory");
    const std::string bytes(path);
    return std::filesystem::path(std::u8string(bytes.begin(), bytes.end()));
}
}

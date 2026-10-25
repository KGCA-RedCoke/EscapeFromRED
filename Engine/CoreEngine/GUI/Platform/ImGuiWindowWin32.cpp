#include "ImGuiWindowWin32.h"
#include "Core/Window/Window.h"
#include "Platform/Win32/NativeWindow.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <stdexcept>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

void InitializeImGuiWindowWin32(Window& window)
{
    if (!ImGui_ImplWin32_Init(Jacob::Platform::Win32Handle(window.GetNativeHandle())))
        throw std::runtime_error("ImGui Win32 initialization failed.");
    window.SetNativeEventFilter([](const Jacob::Platform::NativeEvent& event)
    {
        return event.kind == Jacob::Platform::NativeWindowKind::Win32 && ImGui::GetCurrentContext() &&
               ImGui_ImplWin32_WndProcHandler(static_cast<HWND>(event.window), event.message,
                                             event.wparam, event.lparam) != 0;
    });
}

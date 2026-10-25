#include "Core/Window/Window.h"
#include "GUI/Platform/ImGuiWindowWin32.h"
#include "Platform/Win32/NativeWindow.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <iostream>
#include <stdexcept>

// Exercises the actual legacy-editor adapter without FBX or a graphics device.
// ImGui produces CPU draw lists here; this is not a D3D rendering test.
int main()
{
    try
    {
        Window window("Jacob ImGui platform bridge", {640, 360, false, true});
        window.Initialize();
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        unsigned char* pixels = nullptr;
        int width = 0, height = 0;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        if (!pixels || width <= 0 || height <= 0) throw std::runtime_error("Font atlas initialization failed.");
        InitializeImGuiWindowWin32(window);

        for (int frame = 0; frame < 8; ++frame)
        {
            window.Update();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Editor platform test");
            ImGui::TextUnformatted("Window and ImGui integration without a renderer");
            ImGui::End();
            ImGui::Render();
        }
        if (io.DisplaySize.x <= 0 || io.DisplaySize.y <= 0 || ImGui::GetDrawData()->CmdListsCount == 0)
            throw std::runtime_error("ImGui did not receive the window dimensions or produce draw lists.");

        // WM_CHAR must reach the real adapter installed on the platform window.
        const auto handle = Jacob::Platform::Win32Handle(window.GetNativeHandle());
        PostMessageW(handle, WM_CHAR, L'A', 0);
        window.Update();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        bool foundCharacter = false;
        for (const auto character : io.InputQueueCharacters) foundCharacter |= character == L'A';
        ImGui::EndFrame();
        if (!foundCharacter) throw std::runtime_error("Native text input did not reach ImGui.");

        PostMessageW(handle, WM_CLOSE, 0, 0);
        window.Update();
        if (!window.IsClosed() || !IsWindow(handle))
            throw std::runtime_error("Window closed before ImGui could release its borrowed handle.");
        window.SetNativeEventFilter({});
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        std::cout << "ImGui Win32 bridge: initialization, CPU draw lists, native text input and shutdown passed.\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

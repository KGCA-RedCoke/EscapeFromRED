#include "common_pch.h"
#include "GUI_Chat.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

#include "Core/Graphics/GraphicDevice.h"
#include "Core/Window/Window.h"

void GUI_Chat::Initialize()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(Window::GetWindow()->GetWindowHandle());
    ImGui_ImplDX11_Init(G_Context.GetDevice(), G_Context.GetImmediateDeviceContext());
}

void GUI_Chat::Update(float DeltaTime)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Chatting
    {
        ImGui::Begin("Chatting");

        static char                buffer[1024 * 16];
        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        ImGui::InputTextMultiline(
            "##source",
            buffer,
            IM_ARRAYSIZE(buffer),
            ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16),
            flags);

        if (ImGui::Button("Send", ImVec2(60, 30)))
        {
            LOG_CORE_INFO("Pushed");
        }

        ImGui::End();
    } {}
}

void GUI_Chat::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GUI_Chat::Release()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

#include "common_pch.h"
#include "GUI_Viewport.h"

#include "Core/Entity/Camera/JCamera.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/MManagerInterface.h"


GUI_Viewport::GUI_Viewport(const std::string& InTitle)
	: GUI_Base(InTitle),
	  bIsFocused(false),
	  bIsHovered(false) {}

GUI_Viewport::~GUI_Viewport()
{}

void GUI_Viewport::Initialize()
{
	mEditorCameraRef = IManager.CameraManager.GetCam(L"EditorCamera");
	assert(mEditorCameraRef);
}

void GUI_Viewport::Update(float DeltaTime)
{
	ImGui::Begin(mTitle.c_str());
	{
		bIsFocused = ImGui::IsWindowFocused();
		bIsHovered = ImGui::IsWindowHovered();

		ImVec2 curSize = ImGui::GetContentRegionAvail();

		FViewportData* viewportData = IManager.ViewportManager.FetchResource(Name_Editor_Viewport);
		if (!viewportData)
		{
			ImGui::End();
			LOG_CORE_FATAL("Invalid Viewport");
		}

		ImGui::Image(viewportData->SRV.Get(), curSize);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
		{
			mEditorCameraRef->Update(DeltaTime);
		}

		ImGui::End();
	}
}

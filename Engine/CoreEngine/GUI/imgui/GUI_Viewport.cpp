#include "GUI_Viewport.h"

#include "Core/Entity/Camera/JCamera.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/MManagerInterface.h"


GUI_Viewport::GUI_Viewport(const std::string& InTitle)
	: GUI_Base(InTitle),
	  bIsFocused(false),
	  bIsHovered(false),
	  mEditorCameraRef(nullptr),
	  mCachedViewportSize(FVector2::ZeroVector) {}

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

		FViewportData* viewportData = IManager.ViewportManager.FetchResource(Name_Editor_Viewport);
		if (!viewportData)
		{
			ImGui::End();
			LOG_CORE_FATAL("Invalid Viewport");
		}

		if (mCachedViewportSize == FVector2::ZeroVector)
		{
			mCachedViewportSize = FVector2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		}

		// FVector2 curSize = FVector2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		//
		// constexpr float targetAspectRatio  = 16.f / 9.f;
		// float           currentAspectRatio = curSize.x / curSize.y;
		//
		// if (currentAspectRatio > targetAspectRatio)
		// {
		// 	curSize.x = curSize.y * targetAspectRatio;
		// }
		// else
		// {
		// 	curSize.y = curSize.x / targetAspectRatio;
		// }
		//
		// float offsetX = (ImGui::GetContentRegionAvail().x - curSize.x) * 0.5f;
		// float offsetY = (ImGui::GetContentRegionAvail().y - curSize.y) * 0.5f;
		//
		// if (!mCachedViewportSize.IsNearlyEqual(curSize))
		// {
		// 	IManager.ViewportManager.ResizeViewport(Name_Editor_Viewport, curSize.x, curSize.y, offsetX, offsetY);
		// 	mCachedViewportSize = curSize;
		// }

		ImGui::Image(viewportData->SRV.Get(), ImGui::GetContentRegionAvail());

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
		{
			mEditorCameraRef->Update(DeltaTime);
		}

		ImGui::End();
	}
}

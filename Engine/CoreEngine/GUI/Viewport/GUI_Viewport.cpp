#include "GUI_Viewport.h"

#include "Core/Entity/Camera/JCamera.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/MManagerInterface.h"


GUI_Viewport::GUI_Viewport(const JText& InTitle)
	: GUI_Base(InTitle),
	  bIsFocused(false),
	  bIsHovered(false),
	  mEditorCameraRef(nullptr),
	  mViewportTitle(InTitle),
	  mCachedViewportSize(FVector2::ZeroVector) {}

void GUI_Viewport::Update_Implementation(float DeltaTime)
{
	bIsFocused = ImGui::IsWindowFocused();
	bIsHovered = ImGui::IsWindowHovered();
	// Name_Editor_Viewport
	Ptr<FViewportData> viewportData = IManager.ViewportManager->FetchResource(mViewportTitle);
	if (!viewportData)
	{
		ImGui::End();
		LOG_CORE_FATAL("Invalid Viewport");
	}

	if (mCachedViewportSize == FVector2::ZeroVector)
	{
		mCachedViewportSize = FVector2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	}

	ImGui::Image(viewportData->SRV.Get(), ImGui::GetContentRegionAvail());

	if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
	{
		if (mEditorCameraRef)
			mEditorCameraRef->Update(DeltaTime);
	}
}

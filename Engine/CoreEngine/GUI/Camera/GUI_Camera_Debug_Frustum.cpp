#include "GUI_Camera_Debug_Frustum.h"

#include "Core/Interface/MManagerInterface.h"

GUI_Camera_Debug_Frustum::GUI_Camera_Debug_Frustum(const JText& InTitle)
	: GUI_Base(InTitle)
{
	// GUI_Camera_Debug_Frustum::Initialize();
}

void GUI_Camera_Debug_Frustum::Update_Implementation(float DeltaTime)
{
	auto EditorViewport = IManager.ViewportManager->FetchResource(Name_Editor_Viewport);
	assert(EditorViewport);

	mSampleTexture = EditorViewport->SRV;
	assert(mSampleTexture);
	ImGui::Image(mSampleTexture.Get(), ImGui::GetContentRegionAvail());
}

void GUI_Camera_Debug_Frustum::Initialize()
{
	
}

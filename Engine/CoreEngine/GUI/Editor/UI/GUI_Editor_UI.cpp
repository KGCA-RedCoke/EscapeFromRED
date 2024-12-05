#include "GUI_Editor_UI.h"

#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"

GUI_Editor_UI::GUI_Editor_UI(const JText& InTitle)
	: GUI_Editor_Base(InTitle)
{}

void GUI_Editor_UI::ShowMenuBar()
{
	GUI_Editor_Base::ShowMenuBar();

	if (ImGui::BeginMenuBar())
	{

		ImGui::EndMenuBar();
	}
}

void GUI_Editor_UI::Update_Implementation(float DeltaTime)
{
	GUI_Editor_Base::Update_Implementation(DeltaTime);

	if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
	{
		if (mCamera)
		{
			mCamera->Tick(mDeltaTime);
		}
	}
}

void GUI_Editor_UI::Render()
{
	GUI_Editor_Base::Render();

	G_DebugBatch.PreRender();
	G_DebugBatch.DrawGrid_Implement(
									{mWindowSize.x, 0},
									{0, mWindowSize.y},
									{0, 0},
									16,
									16,
									Colors::Blue
								   );
	G_DebugBatch.PostRender();
}


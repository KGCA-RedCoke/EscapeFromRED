#include "GUI_Editor_UI.h"

#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"

GUI_Editor_UI::GUI_Editor_UI(const JText& InTitle)
	: GUI_Editor_Base(InTitle)
{}

void GUI_Editor_UI::Update_Implementation(float DeltaTime)
{
	GUI_Editor_Base::Update_Implementation(DeltaTime);

	
	
}

void GUI_Editor_UI::Render()
{
	GUI_Editor_Base::Render();

	G_DebugBatch.DrawGrid_Implement(
									{},
									{},
									{},
									{},
									{},
									{}
								   );
}

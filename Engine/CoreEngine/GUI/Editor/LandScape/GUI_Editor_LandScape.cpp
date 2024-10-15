#include "GUI_Editor_LandScape.h"

#include "Core/Window/Application.h"

GUI_Editor_LandScape::GUI_Editor_LandScape(const JText& InTitle)
	: GUI_Base(InTitle) {}

GUI_Editor_LandScape::~GUI_Editor_LandScape() {}

void GUI_Editor_LandScape::Render()
{
	GUI_Base::Render();
}

void GUI_Editor_LandScape::Update_Implementation(float DeltaTime)
{
	ImGui::DragInt("Columns", &mLandScapeDesc.Column, 1, 1, 100);
	ImGui::DragInt("Rows", &mLandScapeDesc.Row, 1, 1, 100);
	ImGui::DragFloat("Width", &mLandScapeDesc.CellDistance, 1.f, 1.f, 1000.f);
	ImGui::DragFloat("Height", &mLandScapeDesc.ScaleHeight, 1.f, 1.f, 1000.f);

	ImGui::Separator();

	if (ImGui::Button("Generate LandScape"))
	{
		GenerateLandScape();
	}
}

void GUI_Editor_LandScape::GenerateLandScape()
{
	Ptr<JLandScape> newLandScape  = MakePtr<JLandScape>("LandScape");
	newLandScape->mMapDescription = mLandScapeDesc;

	newLandScape->Initialize();
	newLandScape->GenerateLandScape();

	Application::s_AppInstance->Actors.push_back(newLandScape);
}

#include "common_pch.h"
#include "GUI_Inspector.h"

GUI_Inspector::GUI_Inspector(const std::string& InTitle)
	: GUI_Base(InTitle) {}


void GUI_Inspector::Initialize()
{
	GUI_Base::Initialize();
}

void GUI_Inspector::Update(float_t DeltaTime)
{
	GUI_Base::Update(DeltaTime);
	ImGui::Begin(mTitle.c_str(), &bIsWindowOpen);
	{

		ImGui::End();
	}
}

void GUI_Inspector::Release()
{
	GUI_Base::Release();
}

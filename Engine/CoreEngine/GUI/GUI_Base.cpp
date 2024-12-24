﻿#include "GUI_Base.h"
#include "imgui/imgui_internal.h"

GUI_Base::GUI_Base()
{}

GUI_Base::GUI_Base(const JText& InTitle)
	: mTitle(InTitle),
	  bIsWindowOpen(true)
{}

void GUI_Base::Update(float DeltaTime)
{
	if (!bIsWindowOpen)
		return;


	ImGui::Begin(mTitle.c_str(), &bIsWindowOpen, mWindowFlags);
	{
		bIsVisible = ImGui::IsItemVisible();

		if (!ImGui::IsWindowCollapsed())
		{
			ChangeWindowStyleIfNotDocked();

			ShowMenuBar();

			Update_Implementation(DeltaTime);
		}


		ImGui::End();
	}
}

bool GUI_Base::IsWindowDocked() const
{
	const ImGuiID        dockNodeID = ImGui::GetID(mTitle.c_str());
	const ImGuiDockNode* node       = ImGui::DockBuilderGetNode(dockNodeID);

	return node != nullptr;
}

void GUI_Base::OpenIfNotOpened()
{
	if (!bIsWindowOpen)
	{
		bIsWindowOpen = true;
	}
}

void GUI_Base::ChangeWindowStyleIfNotDocked()
{
	if (!IsWindowDocked())
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 45.5f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}

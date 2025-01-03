﻿#include "GUI_Editor_Node.h"

#include "Core/Utils/HelperFunction.h"
#include "Core/Utils/Utils.h"


GUI_Editor_Node::GUI_Editor_Node(const JText& InTitle)
	: GUI_Base(InTitle)
{
	mWindowFlags |= ImGuiWindowFlags_MenuBar;

	GUI_Editor_Node::Initialize();
}

void GUI_Editor_Node::Initialize()
{
	edC::Config config;

	mSaveFilePath       = std::format("Configs/{}.json", ParseFile(mTitle));
	config.SettingsFile = mSaveFilePath.c_str();
	mContext            = edC::CreateEditor(&config);
}

void GUI_Editor_Node::Update(float DeltaTime)
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

			edC::SetCurrentEditor(mContext);
			edC::Begin(mTitle.c_str(), ImVec2(0.0, 0.0f));
			{
				Update_Implementation(DeltaTime);
			}
			
			edC::End();
			edC::SetCurrentEditor(nullptr);
		}


		ImGui::End();
	}
}

void GUI_Editor_Node::Update_Implementation(float DeltaTime)
{
	for (auto& node : mNodes)
	{
		node->Update();
	}

	/*for (auto& link : mLinks)
		ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);*/
}

void GUI_Editor_Node::Release()
{
	edC::DestroyEditor(mContext);
}

void GUI_Editor_Node::UpdateTouch(float DeltaTime)
{
	for (auto& entry : mNodeTouchTime)
	{
		if (entry.second > 0.0f)
			entry.second -= DeltaTime;
	}
}

void GUI_Editor_Node::BuildNode(edC::FNode* InNode)
{
	for (auto& input : InNode->Inputs)
	{
		input.Node = InNode;
		input.Kind = edC::PinKind::Input;
	}

	for (auto& output : InNode->Outputs)
	{
		output.Node = InNode;
		output.Kind = edC::PinKind::Output;
	}
}

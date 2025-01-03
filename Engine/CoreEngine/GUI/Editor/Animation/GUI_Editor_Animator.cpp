﻿#include "GUI_Editor_Animator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/FileIO/JSerialization.h"

GUI_Editor_Animator::GUI_Editor_Animator(const JText& InTitle)
	: GUI_Editor_Node(InTitle)
{
	mAnimator = MAnimatorManager::Get().Load(InTitle, nullptr);
	assert(mAnimator);

	for (auto& node : mAnimator->mStateMachine)
	{
		SpawnAnimSequenceNode(node.first, node.second->GetName());
	}
}

void GUI_Editor_Animator::Initialize()
{
	GUI_Editor_Node::Initialize();
}

void GUI_Editor_Animator::Render()
{
	GUI_Editor_Node::Render();
}

void GUI_Editor_Animator::Release()
{
	GUI_Editor_Node::Release();
}

void GUI_Editor_Animator::Update(float DeltaTime)
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

			DrawDetails(396.f);

			ImGui::SameLine(0, 12.f);

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

void GUI_Editor_Animator::Update_Implementation(float DeltaTime)
{
	if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
		const char*         str     = static_cast<const char*>(payload->Data);

		auto metaData = Utils::Serialization::GetType(str);

		if (metaData.AssetType == HASH_ASSET_TYPE_ANIMATION_CLIP)
		{
			auto openPopupPosition = ImGui::GetMousePos();

			edC::FNode* node = SpawnAnimSequenceNode("___", ParseFile(str));
			node->Pos       = ImVec2(openPopupPosition.x, openPopupPosition.y);
			edC::SetNodePosition(node->ID, node->Pos);

			ImGui::EndDragDropTarget();
		}
	}

	GUI_Editor_Node::Update_Implementation(DeltaTime);

	for (auto& node : mNodes)
	{
		for (auto& link : node->Links)
		{
			edC::Link(link.ID, link.StartPin, link.EndPin);
		}
	}

}

void GUI_Editor_Animator::ShowMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(u8("파일")))
		{
			if (ImGui::MenuItem(u8("저장")))
			{
				for (auto& node : mNodes)
				{
					/*for (auto& link : node->Links)
					{
						edC::Link(link.ID, link.StartPin, link.EndPin);
					}*/

					mAnimator->AddAnimationClip(node->Title, node->Name);
				}

				Utils::Serialization::Serialize(mTitle.c_str(), mAnimator);
			}

			ImGui::EndMenu();
		}
	}
	ImGui::EndMenuBar();
}

void GUI_Editor_Animator::DrawDetails(float paneWidth)
{
	ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

	paneWidth = ImGui::GetContentRegionAvail().x;

	ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
	ImGui::Spring(0.0f, 0.0f);
	if (ImGui::Button("Zoom to Content"))
	{
		edC::NavigateToContent();

	}
	ImGui::Spring();

	ImGui::EndHorizontal();
	ImGui::EndChild();
}

edC::FNode* GUI_Editor_Animator::SpawnAnimSequenceNode(const JText& InState, const JText& InAnimClipText)
{
	auto       newNode = MakeUPtr<edC::FNode>(InAnimClipText);
	edC::FNode* node    = newNode.get();

	uint32_t inputHash  = StringHash(std::format("{}_Input", InAnimClipText).c_str());
	uint32_t outputHash = StringHash(std::format("{}_Output", InAnimClipText).c_str());

	newNode->Type = edC::NodeType::AnimSequence;
	newNode->Inputs.emplace_back(edC::FPin(inputHash, "", edC::PinType::Flow));
	newNode->Outputs.emplace_back(edC::FPin(outputHash, "", edC::PinType::Flow));
	newNode->Title = InState;

	BuildNode(node);

	mNodes.emplace_back(std::move(newNode));

	return node;
}

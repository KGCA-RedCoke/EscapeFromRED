#include "GUI_Editor_Animator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/FileIO/JSerialization.h"

GUI_Editor_Animator::GUI_Editor_Animator(const JText& InTitle)
	: GUI_Editor_Node(InTitle)
{
	mAnimator = MAnimatorManager::Get().Load(InTitle, nullptr);
	assert(mAnimator);
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

			ed::FNode* node = SpawnAnimSequenceNode(ParseFile(str));
			node->Pos       = ImVec2(openPopupPosition.x, openPopupPosition.y);
			ed::SetNodePosition(node->ID, node->Pos);

			// mAnimator->AddAnimationClip()

			ImGui::EndDragDropTarget();
		}
	}

	GUI_Editor_Node::Update_Implementation(DeltaTime);

	for (auto& node : mNodes)
	{
		for (auto& link : node->Links)
		{
			ed::Link(link.ID, link.StartPin, link.EndPin);
		}
	}
}

void GUI_Editor_Animator::ShowMenuBar()
{
	GUI_Editor_Node::ShowMenuBar();
}

ed::FNode* GUI_Editor_Animator::SpawnAnimSequenceNode(const JText& InName)
{
	auto       newNode = MakeUPtr<ed::FNode>(InName);
	ed::FNode* node    = newNode.get();

	uint32_t inputHash  = StringHash(std::format("{}_Input", InName).c_str());
	uint32_t outputHash = StringHash(std::format("{}_Output", InName).c_str());

	newNode->Type = ed::NodeType::AnimSequence;
	newNode->Inputs.emplace_back(ed::FPin(inputHash, "", ed::PinType::Flow));
	newNode->Outputs.emplace_back(ed::FPin(outputHash, "", ed::PinType::Flow));

	BuildNode(node);

	mNodes.emplace_back(std::move(newNode));

	return node;
}

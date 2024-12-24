#include "GUI_Node.h"

#include "Core/Utils/Logger.h"
#include "Core/Utils/Utils.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_node_editor_internal.h"
#include "imgui/imgui_stdlib.h"

namespace ax::NodeEditor
{
	FNode::FNode(JText InName)
		: Name(std::move(InName))
	{
		ID     = StringHash(Name.c_str());
		NodeID = ID;
	}

	void FNode::Update()
	{
		BeginNode(ID);

		// 제목 출력 (편집 중이 아닌 경우)
		if (!bIsEditing)
		{
			ImGui::TextColored({1.f, 0.69f, 0.2f, 1.f}, Title.c_str());

			// 더블클릭 시 편집 모드로 전환
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				bIsEditing = true;
			}
		}
		else
		{
			// 편집 모드: 입력 창 표시
			ImGui::InputText("##EditTitle", &Title);


			// 입력 창 이외의 영역 클릭 시 편집 모드 종료
			if (!ImGui::IsItemActive() && ImGui::IsMouseClicked(0))
			{
				bIsEditing = false;
			}
		}

		ImGui::Text(Name.c_str());
		Update_Impl();
		EndNode();

		if (BeginCreate())
		{
			PinId inputPinId, outputPinId;
			if (QueryNewLink(&inputPinId, &outputPinId))
			{
				//   * input valid, output invalid - user started to drag new ling from input pin
				//   * input invalid, output valid - user started to drag new ling from output pin
				//   * input valid, output valid   - user dragged link over other pin, can be validated
				if (inputPinId && outputPinId)
				{
					if (AcceptNewItem())
					{
						Links.push_back(FLink(Links.size(), inputPinId, outputPinId));
						Link(Links.back().ID, Links.back().StartPin, Links.back().EndPin);
					}
				}
			}
		}
		EndCreate();

		if (BeginDelete())
		{
			// There may be many links marked for deletion, let's loop over them.
			LinkId deletedLinkId;
			while (QueryDeletedLink(&deletedLinkId))
			{
				// If you agree that link can be deleted, accept deletion.
				if (AcceptDeletedItem())
				{
					// Then remove link from your data.
					for (auto& link : Links)
					{
						if (link.ID == deletedLinkId)
						{
							Links.erase(&link);
							break;
						}
					}
				}
			}
			LOG_CORE_INFO("Delete");
		}
		EndDelete();


	}

	void FNode::Update_Impl()
	{
		BeginPin(Inputs[0].ID, PinKind::Input);
		ImGui::Text("-> In");
		EndPin();

		ImGui::SameLine();

		BeginPin(Outputs[0].ID, PinKind::Output);
		ImGui::Text("Out ->");
		EndPin();


	}

	FLink::FLink(LinkId InID, PinId InStartPin, PinId InEndPin)
		: ID(InID),
		  StartPin(InStartPin),
		  EndPin(InEndPin) {}
}

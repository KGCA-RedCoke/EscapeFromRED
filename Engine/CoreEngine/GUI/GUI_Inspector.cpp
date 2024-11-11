#include "GUI_Inspector.h"

#include "Core/Entity/Component/Scene/JSceneComponent.h"
#include "Core/Graphics/ShaderStructs.h"

extern CBuffer::Light g_LightData;

GUI_Inspector::GUI_Inspector(const std::string& InTitle)
	: GUI_Base(InTitle) {}

void GUI_Inspector::AddSceneComponent(const JText& InName, const Ptr<JSceneComponent>& InSceneComponent)
{
	mSceneComponents.try_emplace(InName, InSceneComponent);
}

void GUI_Inspector::Update_Implementation(float DeltaTime)
{
	DrawSearchBar();

	if (ImGui::CollapsingHeader("Light Settings"))
	{
		ImGui::ColorEdit3("Directional Light Color", &g_LightData.LightColor.x);
		ImGui::DragFloat3("Directional Light Position", &g_LightData.LightPos.x, 0.05f);

		ImGui::Separator();
	}

	if (ImGui::BeginTable("bg", 1, ImGuiTableFlags_RowBg))
	{
		for (auto& element : mSceneComponents)
		{
			if (auto sceneComp = element.second.lock())
			{
				if (mFilter.PassFilter(sceneComp->GetName().c_str()))
				{
					bool bIsDragging = false;

					DrawTreeNode(sceneComp);

					if (ImGui::CollapsingHeader(sceneComp->GetName().c_str()))
					{
						FVector location = sceneComp->GetLocalLocation();
						FVector rotation = sceneComp->GetLocalRotation();
						FVector scale    = sceneComp->GetLocalScale();

						if (ImGui::DragFloat3("Position", &location.x, 1.f))
						{
							bIsDragging = true;
						}

						if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f, -360.0f, 360.0f))
						{
							bIsDragging = true;
						}
						if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.f, 10.0f))
						{
							bIsDragging = true;
						}

						if (bIsDragging)
						{
							sceneComp->SetLocalLocation(location);
							sceneComp->SetLocalRotation(rotation);
							sceneComp->SetLocalScale(scale);
						}
					}
				}
			}
		}
		ImGui::EndTable();
	}

}

void GUI_Inspector::DrawSearchBar()
{
	// 검색 바 크기 조정
	ImGui::SetNextItemWidth(-FLT_MIN);

	// 바로가기 단축키 설정 (Ctrl + F), 마우스 호버 시 툴팁 표시
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F, ImGuiInputFlags_Tooltip);

	// 기본적 포커스를 받지 않음
	ImGui::PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);

	if (ImGui::InputTextWithHint("##Filter",
								 "incl, -excl (...)",
								 mFilter.InputBuf,
								 IM_ARRAYSIZE(mFilter.InputBuf),
								 ImGuiInputTextFlags_EscapeClearsAll))
	{
		mFilter.Build();
	}
	ImGui::PopItemFlag();
}

void GUI_Inspector::DrawTreeNode(const Ptr<JSceneComponent>& InSceneComponent)
{
	// 테이블의 다음 행, 다음 열로 이동
	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	// 아이템 ID를 씬 컴포넌트 루트 이름으로 설정
	ImGui::PushID(InSceneComponent->GetHash());

	// 클릭시 발생 이벤트 설정
	ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_None;
	treeNodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;			// 화살표 클릭시 열림
	treeNodeFlags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;		// 더블클릭시 열림
	treeNodeFlags |= ImGuiTreeNodeFlags_NavLeftJumpsBackHere;	// 왼쪽 화살표로 점프

	if (mSelectedSceneComponent == InSceneComponent)	// 선택된 노드
	{
		treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	if (InSceneComponent->GetChildCount() == 0)			// 자식이 없는 노드
	{
		treeNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
	}
	if (!InSceneComponent->IsActivated())				// 비활성화된 노드
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
	}
	if (ImGui::IsItemFocused())
	{
		mSelectedSceneComponent = InSceneComponent;
	}

	const bool bIsOpen = ImGui::TreeNodeEx("", treeNodeFlags, "%s", InSceneComponent->GetName().c_str());
	if (bIsOpen)
	{
		for (auto& child : InSceneComponent->GetChildSceneComponents())
		{
			if (auto ptr = child.lock())
			{
				DrawTreeNode(ptr);
			}
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}

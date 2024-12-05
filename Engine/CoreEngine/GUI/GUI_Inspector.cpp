#include "GUI_Inspector.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Actor/JStaticMeshActor.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/JWorld.h"

extern CBuffer::Light g_LightData;

GUI_Inspector::GUI_Inspector(const std::string& InTitle)
	: GUI_Base(InTitle) {}

void GUI_Inspector::AddSceneComponent(const JText& InName, AActor* InSceneComponent)
{
	mSceneComponents.try_emplace(InName, InSceneComponent);
}

void GUI_Inspector::Update_Implementation(float DeltaTime)
{
	ImGui::BeginChild("Hierarchy", ImVec2(0, 0), ImGuiChildFlags_ResizeY);

	DrawSearchBar();
	if (ImGui::CollapsingHeader("Light Settings"))
	{
		ImGui::ColorEdit3("Directional Light Color", &g_LightData.LightColor.x);
		ImGui::DragFloat3("Directional Light Position", &g_LightData.LightPos.x, 0.05f);

		ImGui::Separator();
	}

	if (ImGui::BeginTable("bg", 1, ImGuiTableFlags_RowBg))
	{
		mLevel = GetWorld.LevelManager->GetActiveLevel();

		for (auto& element : mLevel->mActors)
		{
			if (auto actor = element.get())
			{
				if (mSearchBar.Filter.PassFilter(actor->GetName().c_str()))
				{
					DrawTreeNode(actor);
				}
			}
		}
		ImGui::EndTable();

		if (mSelectedSceneComponent && ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			mSelectedSceneComponent->Destroy();
			mSelectedSceneComponent = nullptr;
		}

		if (mSelectedSceneComponent && ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_C))
		{

			mLevel->mActors.push_back(UPtrCast<JStaticMeshActor>(mSelectedSceneComponent->Clone()));
		}
	}

	ImGui::EndChild();

	ImGui::Separator();

	DrawDetails();
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
								 mSearchBar.Filter.InputBuf,
								 IM_ARRAYSIZE(mSearchBar.Filter.InputBuf),
								 ImGuiInputTextFlags_EscapeClearsAll))
	{
		mSearchBar.Filter.Build();
	}
	ImGui::PopItemFlag();
}

void GUI_Inspector::DrawTreeNode(JSceneComponent* InSceneComponent)
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

	const bool bIsOpen = ImGui::TreeNodeEx("", treeNodeFlags, "%s", InSceneComponent->GetName().c_str());
	if (ImGui::IsItemClicked())
	{
		mSelectedSceneComponent = InSceneComponent;
	}


	if (bIsOpen)
	{
		for (int32_t i = 0; i < InSceneComponent->GetChildCount(); ++i)
		{
			DrawTreeNode(InSceneComponent->mChildSceneComponents[i].get());
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void GUI_Inspector::DrawDetails()
{
	// 하위 그룹 생성
	if (ImGui::BeginChild("Details", ImVec2(0, 200), ImGuiChildFlags_ResizeY))
	{
		if (mSelectedSceneComponent == nullptr)
		{
			ImGui::Text(u8("선택된 SceneComponent가 없음."));
			ImGui::EndChild();
			return;
		}

		if (ImGui::CollapsingHeader(mSelectedSceneComponent->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (mSelectedSceneComponent)
			{
				mSelectedSceneComponent->ShowEditor();
			}
		}
	}
	ImGui::EndChild();

}

void GUI_Inspector::DrawName()
{}

void GUI_Inspector::DrawTransform() const
{
	if (mSelectedSceneComponent)
	{
		mSelectedSceneComponent->ShowEditor();
	}
}

void GUI_Inspector::DrawFlags()
{}

void GUI_Inspector::DrawMaterialSlot()
{
	// if (JStaticMeshActor* staticMeshActor = dynamic_cast<JStaticMeshActor*>(mSelectedSceneComponent))
	// {
	// 	if (ImGui::CollapsingHeader(u8("머티리얼")))
	// 	{
	// 		for (int32_t i = 0; i < staticMeshActor->GetMaterialCount(); ++i)
	// 		{
	// 			staticMeshActor->SetMaterialInstance();
	// 		}
	// 	}
	// }
}

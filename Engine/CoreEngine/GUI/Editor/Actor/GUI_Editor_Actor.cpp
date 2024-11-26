#include "GUI_Editor_Actor.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Actor/MActorManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"


GUI_Editor_Actor::GUI_Editor_Actor(const JText& InPath)
	: GUI_Editor_Base(InPath),
	  mSelectedSceneComponent(nullptr)
{
	mActorToEdit = GetWorld.ActorManager->CreateOrLoad(InPath);

	if (!Utils::Serialization::IsJAssetFileAndExist(InPath.c_str()))
	{
		Utils::Serialization::Serialize(InPath.c_str(), mActorToEdit);
	}

	mViewport = MViewportManager::Get().CreateOrLoad(mTitle, 1280, 720);
	assert(mViewport);

	mCamera = MCameraManager::Get().CreateOrLoad<JCamera_Debug>(mTitle);
	assert(mCamera);
}

void GUI_Editor_Actor::Initialize()
{
	GUI_Base::Initialize();


}

void GUI_Editor_Actor::Update_Implementation(float DeltaTime)
{
	mDeltaTime = DeltaTime;

	if (mActorToEdit == nullptr)
	{
		return;
	}

	DrawHierarchy();

	DrawViewport();

	DrawDetails();
}

void GUI_Editor_Actor::Render()
{
	using Super = GUI_Editor_Base;

	Super::Render();
}

void GUI_Editor_Actor::DrawHierarchy()
{
	// 가장 위에 Add Button 추가
	// Add Button을 누르면 리스트 박스가 나오게
	// 리스트 박스에는 추가할 수 있는 컴포넌트들이 나오게
	// 가장 먼저 컴포넌트는 스태틱, 스켈레탈 메시, 액터를 추가할 수 있게 구현

	// 계층도에서는 가장먼저 RootComponent를 표현


	if (ImGui::BeginChild("Actor Hierarchy", ImVec2(200, 0), ImGuiChildFlags_ResizeX))
	{
		ImGui::Text("Actor Hierarchy");

		if (ImGui::BeginTable("Hierarchy", 1, ImGuiTableFlags_RowBg))
		{
			// Root Actor
			DrawTreeNode(mActorToEdit);

			// Child Components
			// for (auto& component : mActorToEdit->GetChildSceneComponents())
			// {
			// 	if (component)
			// 	{
			// 		DrawTreeNode(component);
			// 	}
			// }
			ImGui::EndTable();
		}
	}
	ImGui::EndChild();
}

void GUI_Editor_Actor::DrawTreeNode(JSceneComponent* InSceneComponent)
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
	// if (InSceneComponent->GetChildCount() == 0)			// 자식이 없는 노드
	// {
	// 	treeNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
	// }
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
		// for (auto& child : InSceneComponent->GetChildSceneComponents())
		// {
		// 	if (child)
		// 	{
		// 		DrawTreeNode(child);
		// 	}
		// }
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void GUI_Editor_Actor::DrawViewport() const
{
	ImGui::SameLine();

	if (ImGui::BeginChild("ActorView", ImVec2(400, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border))
	{
		ImGui::Image(mViewport->SRV.Get(), ImGui::GetContentRegionAvail());

		if (ImGui::IsItemHovered() && ImGui::IsItemFocused())
		{
			mCamera->Update(mDeltaTime);
		}
	}
	ImGui::EndChild();
}

void GUI_Editor_Actor::DrawDetails()
{
	ImGui::SameLine();

	ImGui::BeginGroup();
	if (ImGui::BeginTable("##Property", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
	{
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

		if (mSelectedSceneComponent)
		{
			ImGui::TableNextRow();
			ImGui::PushID("Location");
			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Location");
			ImGui::TableNextColumn();
			ImGui::DragFloat3("##Location", &mSelectedSceneComponent->mLocalLocation.x, 0.01f, -100.f, 100.0f);

			ImGui::TableNextRow();
			ImGui::PushID("Rotation");
			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Rotation");
			ImGui::TableNextColumn();
			ImGui::DragFloat3("##Rotation", &mSelectedSceneComponent->mLocalRotation.x, 0.01f, -100.f, 100.0f);

			// 여기에 컴포넌트 정보를 표시
			// 근데 컴포넌트의 어떤 정보를 표시할지는 (어떻게 구분할지)
			// 언리얼의 UProperty인데 이걸 어떻게 구현할지는 모르겠다.
			ImGui::TableNextRow();
			ImGui::PushID("Scale");
			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Scale");
			ImGui::TableNextColumn();
			ImGui::DragFloat3("##Scale", &mSelectedSceneComponent->mLocalScale.x, 0.01f, 0.f, 10.0f);
		}

		ImGui::EndTable();
	}

	ImGui::EndGroup();
}

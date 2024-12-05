#include "GUI_Editor_Actor.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Actor/MActorManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Entity/Light/JLightComponent.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"


GUI_Editor_Actor::GUI_Editor_Actor(const JText& InPath)
	: GUI_Editor_Base(InPath),
	  mSelectedSceneComponent(nullptr)
{
	mActorToEdit = GetWorld.ActorManager->Load(InPath);

	if (!Utils::Serialization::IsJAssetFileAndExist(InPath.c_str()))
	{
		Utils::Serialization::Serialize(InPath.c_str(), mActorToEdit);
	}

	mViewport = MViewportManager::Get().Load(mTitle, 1280, 720);
	assert(mViewport);

	mCamera = MCameraManager::Get().Load<JCamera_Debug>(mTitle);
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

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
	{
		Utils::Serialization::Serialize(mTitle.c_str(), mActorToEdit);
	}

	mActorToEdit->Tick(DeltaTime);

	DrawHierarchy();

	DrawViewport();

	DrawDetails();
}

void GUI_Editor_Actor::Render()
{
	using Super = GUI_Editor_Base;

	Super::Render();

	mActorToEdit->Draw();
	MMeshManager::Get().FlushCommandList(G_Device.GetImmediateDeviceContext());
}

void GUI_Editor_Actor::DrawHierarchy()
{
	// 가장 위에 Add Button 추가
	// Add Button을 누르면 리스트 박스가 나오게
	// 리스트 박스에는 추가할 수 있는 컴포넌트들이 나오게
	// 가장 먼저 컴포넌트는 스태틱, 스켈레탈 메시, 액터를 추가할 수 있게 구현
	static int item_selected_idx = 0; // Here we store our selected data as an index.

	static bool item_highlight       = false;
	int         item_highlighted_idx = -1; // Here we store our highlighted data as an index.

	if (ImGui::Button("Add Component"))
	{
		bAddComponentListBox = true;
	}

	if (bAddComponentListBox)
	{
		if (ImGui::BeginListBox("Components"))
		{
			for (int n = 0; n < IM_ARRAYSIZE(g_ComponentList); n++)
			{
				const bool is_selected = (item_selected_idx == n);
				if (ImGui::Selectable(g_ComponentList[n], is_selected))
				{
					bAddComponentListBox = false;

					if (n == 3 && mSelectedSceneComponent)
					{
						auto* light = mActorToEdit->CreateDefaultSubObject<JLightComponent>(g_ComponentList[n],
								 mActorToEdit,
								 mSelectedSceneComponent);
						light->SetupAttachment(mSelectedSceneComponent);
					}
				}

				if (item_highlight && ImGui::IsItemHovered())
					item_highlighted_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}

	// 계층도에서는 가장먼저 RootComponent를 표현
	if (ImGui::BeginChild("Actor Hierarchy", ImVec2(200, 0), ImGuiChildFlags_ResizeX))
	{
		ImGui::TextColored(ImColor(255, 255, 0), "Actor Hierarchy");

		if (ImGui::BeginTable("Hierarchy", 1, ImGuiTableFlags_RowBg))
		{
			DrawTreeNode(mActorToEdit);

			ImGui::EndTable();
		}
	}
	ImGui::EndChild();
}

void GUI_Editor_Actor::DrawTreeNode(JSceneComponent* InSceneComponent)
{
	// 클릭시 발생 이벤트 설정
	ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_None;
	treeNodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;			// 화살표 클릭시 열림
	treeNodeFlags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;		// 더블클릭시 열림
	treeNodeFlags |= ImGuiTreeNodeFlags_NavLeftJumpsBackHere;	// 왼쪽 화살표로 점프

	// 테이블의 다음 행, 다음 열로 이동
	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	// 아이템 ID를 씬 컴포넌트 루트 이름으로 설정
	ImGui::PushID(InSceneComponent->GetHash());

	if (mSelectedSceneComponent == InSceneComponent)	// 선택된 노드
	{
		treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	if (InSceneComponent->GetChildCount() == 0)			// 자식이 없는 노드
	{
		treeNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
	}


	if (ImGui::TreeNodeEx("", treeNodeFlags, "%s", InSceneComponent->GetName().c_str()))
	{
		if (!InSceneComponent->IsActivated())				// 비활성화된 노드
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
		}
		if (ImGui::IsItemFocused() || ImGui::IsItemClicked())
		{
			mSelectedSceneComponent = InSceneComponent;
		}

		if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
			const char*         str     = static_cast<const char*>(payload->Data);

			auto metaData = Utils::Serialization::GetType(str);

			switch (metaData.AssetType)
			{
			case HASH_ASSET_TYPE_STATIC_MESH:
				{
					auto* meshComp = mActorToEdit->CreateDefaultSubObject<JStaticMeshComponent>(ParseFile(str),
							 mActorToEdit,
							 mActorToEdit);
					meshComp->SetMeshObject(str);

					meshComp->SetupAttachment(InSceneComponent);
				}
				break;
			case HASH_ASSET_TYPE_SKELETAL_MESH:
				auto* meshComp = mActorToEdit->CreateDefaultSubObject<JSkeletalMeshComponent>(ParseFile(str),
						 mActorToEdit,
						 mActorToEdit);
				meshComp->SetSkeletalMesh(str);

				meshComp->SetupAttachment(InSceneComponent);
				break;
			}
		}

		for (auto& child : InSceneComponent->mChildSceneComponents)
		{
			if (child)
			{
				DrawTreeNode(child.get());
			}
		}
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

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
		{
			if (mCamera)
			{
				mCamera->Tick(mDeltaTime);
			}
		}
	}
	ImGui::EndChild();
}

void GUI_Editor_Actor::DrawDetails()
{
	ImGui::SameLine();
	if (ImGui::BeginChild("Detail Contents"), ImVec2(0, 0), ImGuiWindowFlags_AlwaysAutoResize)
	{
		if (mSelectedSceneComponent)
		{
			ImGui::TextColored(ImColor(255, 255, 0), "Details");

			ImGui::BeginGroup();
			mSelectedSceneComponent->ShowEditor();
			ImGui::EndGroup();
		}

		ImGui::EndChild();
	}
}

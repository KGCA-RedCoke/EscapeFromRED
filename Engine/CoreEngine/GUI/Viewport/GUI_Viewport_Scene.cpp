#include "GUI_Viewport_Scene.h"

#include "Core/Entity/Actor/JStaticMeshActor.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Graphics/Mesh/JSkeletalMeshObject.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Window/Application.h"

GUI_Viewport_Scene::GUI_Viewport_Scene(const JText& InTitle)
	: GUI_Viewport(InTitle),
	  mPauseIcon(nullptr),
	  mPlayIcon(nullptr) {}


void GUI_Viewport_Scene::Initialize()
{
	// mWindowFlags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;

	mEditorCameraRef = IManager.CameraManager->FetchResource(L"EditorCamera");
	assert(mEditorCameraRef);

	mPauseIcon = IManager.TextureManager->CreateOrLoad(L"rsc/Icons/PauseButton On@2x.png");
	mPlayIcon  = IManager.TextureManager->CreateOrLoad(L"rsc/Icons/PlayButton On@2x.png");

}

void GUI_Viewport_Scene::Update_Implementation(float DeltaTime)
{
	ShowTopMenu();

	using Base = GUI_Viewport;
	Base::Update_Implementation(DeltaTime);

	if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
		{
			const char* str = (const char*)payload->Data;

			auto  metaData = Utils::Serialization::GetType(str);
			JText name     = std::format("{}_{}", ParseFile(str), Application::s_AppInstance->Actors.size());

			if (metaData.AssetType == HASH_ASSET_TYPE_STATIC_MESH)
			{

				auto newActor = MakePtr<JStaticMeshActor>(name, IManager.MeshManager->CreateOrClone(str));
				newActor->Initialize();

				Application::s_AppInstance->Actors.push_back(newActor);

			}
			else if (metaData.AssetType == HASH_ASSET_TYPE_SKELETAL_MESH)
			{
				auto newActor = MakePtr<JActor>();
				newActor->Initialize();

				Ptr<JSkeletalMeshObject>  mesh          = IManager.MeshManager->CreateOrClone<JSkeletalMeshObject>(str);
				Ptr<JStaticMeshComponent> meshComponent = MakePtr<JStaticMeshComponent>(ParseFile(str));
				meshComponent->SetMeshObject(mesh);
				meshComponent->AttachToActor(newActor);

				Application::s_AppInstance->Actors.push_back(newActor);
			}

		}
		ImGui::EndDragDropTarget();
	}
}

void GUI_Viewport_Scene::ShowTopMenu()
{
	ImVec2 availSize = ImGui::GetContentRegionAvail();

	// 창의 가로 크기 구하기
	float windowWidth = availSize.x;

	// ImGui::SetNextWindowPos(ImVec2(0, 0));
	// ImGui::SetNextWindowSize(ImVec2(windowWidth, 50));

	// 버튼 크기와 여백 정의
	float buttonWidth  = 16.0f;  // 각 버튼의 가로 길이
	float buttonHeight = 16.0f; // 각 버튼의 높이
	float spacing      = 4.0f;      // 버튼 사이의 간격

	// 버튼 3개 (Pause, Play, Stop)의 총 가로 길이 계산
	float totalButtonWidth = (buttonWidth * 2) + (spacing * 1);

	// 버튼들을 중앙에 배치하기 위한 시작 위치 계산
	float offsetX = (windowWidth - totalButtonWidth) * 0.5f;

	// 버튼을 왼쪽 여백으로 이동 (중앙 정렬)
	ImGui::SetCursorPosX(offsetX);

	// Pause 버튼
	if (ImGui::ImageButton("GameBtn", mPauseIcon->GetSRV(), ImVec2(buttonWidth, buttonHeight)))
	{
		// Pause 동작 처리
		std::cout << "Game Paused" << std::endl;
	}

	ImGui::SameLine(0, spacing); // 버튼 간격 띄우기

	// Play 버튼
	if (ImGui::ImageButton("PlayBtn",mPlayIcon->GetSRV(), ImVec2(buttonWidth, buttonHeight)))
	{
		// Play 동작 처리
		std::cout << "Game Playing" << std::endl;
	}
}

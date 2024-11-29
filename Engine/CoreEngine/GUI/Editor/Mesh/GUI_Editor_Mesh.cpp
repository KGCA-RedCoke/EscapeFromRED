#include "GUI_Editor_Mesh.h"

#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Viewport/MViewportManager.h"

GUI_Editor_Mesh::GUI_Editor_Mesh(const JText& InTitle)
	: GUI_Editor_Base(InTitle)
{
	GUI_Editor_Mesh::Initialize();
}

void GUI_Editor_Mesh::Render()
{
	GUI_Editor_Base::Render();

	if (mMeshObject)
	{
		mMeshObject->UpdateBuffer();
		mMeshObject->Draw();
	}
}

void GUI_Editor_Mesh::Initialize()
{
	mMeshObject = MMeshManager::Get().CreateOrLoad(mTitle);
}

void GUI_Editor_Mesh::Update_Implementation(float DeltaTime)
{
	GUI_Editor_Base::Update_Implementation(DeltaTime);

	DrawViewport();

	DrawProperty();

	DrawMaterialSlot();

	DrawSaveButton();
}

void GUI_Editor_Mesh::DrawViewport() const
{
	if (ImGui::BeginChild("ActorView", ImVec2(mWindowSize.x * 0.66f, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border))
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

void GUI_Editor_Mesh::DrawProperty() const
{
	ImGui::SameLine();

	if (ImGui::BeginChild("Property",
						  ImVec2(mWindowSize.x * 0.33f, 0),
						  ImGuiChildFlags_AutoResizeX))
	{
		if (mMeshObject)
		{
			// 원시 파일 경로 표시
			ImGui::Text("Mesh Name");
			ImGui::Text(&mMeshObject->mName[0]);

			// 메시 타입 표시
			ImGui::Text("Mesh Type : ");
			ImGui::SameLine();
			ImGui::Text(mMeshObject->mPrimitiveMeshData[0]->GetClassType() == EMeshType::Skeletal ? "Skeletal" : "Static");
		}

	}

	// 창이 안보이는 경우 방지
	ImVec2 size = ImGui::GetWindowSize();
	if (size.x < 50 || size.y < 50)
		ImGui::SetWindowSize(ImVec2(max(size.x, 50), max(size.y, 50)));
}

void GUI_Editor_Mesh::DrawMaterialSlot() const
{

	if (mMeshObject)
	{
		ImGui::Separator();

		ImGui::BeginGroup();

		ImGui::Text("Material Slot");

		const int32_t meshDataSize = mMeshObject->mPrimitiveMeshData.size();
		// for (int32_t i = 0; i < meshDataSize; ++i)
		// {
			int32_t materialSize = mMeshObject->mPrimitiveMeshData[0]->GetSubMaterialNum();

			for (int32_t j = 0; j < materialSize; ++j)
			{
				ImGui::Text(mMeshObject->mMaterialInstances[j]->GetMaterialName().c_str());
				ImGui::ImageButton("MaterialSlot", nullptr, ImVec2(100, 100));
				// ImGui::Separator();
				
				if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
				{
					const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
					const char*         str     = static_cast<const char*>(payload->Data);

					auto metaData = Utils::Serialization::GetType(str);

					if (metaData.AssetType == HASH_ASSET_TYPE_MATERIAL_INSTANCE)
					{
						if (auto matInstancePtr = MMaterialInstanceManager::Get().CreateOrLoad(str))
						{
							mMeshObject->SetMaterialInstance(matInstancePtr, j);
						}
					}
				}
			}
		// }

		ImGui::EndGroup();
	}

}

void GUI_Editor_Mesh::DrawSaveButton() const
{

	if (ImGui::Button("Save"))
	{
		if (mMeshObject)
		{
			Utils::Serialization::Serialize(mTitle.c_str(), mMeshObject);
		}
	}
	ImGui::EndChild();


}

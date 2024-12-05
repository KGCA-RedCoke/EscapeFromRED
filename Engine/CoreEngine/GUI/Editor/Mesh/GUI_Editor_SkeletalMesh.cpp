#include "GUI_Editor_SkeletalMesh.h"

#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/JSkeletalMeshObject.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Viewport/MViewportManager.h"

GUI_Editor_SkeletalMesh::GUI_Editor_SkeletalMesh(const JText& InTitle)
	: GUI_Editor_Base(InTitle)
{
	GUI_Editor_SkeletalMesh::Initialize();
}

void GUI_Editor_SkeletalMesh::Render()
{
	GUI_Editor_Base::Render();

	if (const auto& ptr = mMeshObject)
	{
		ptr->UpdateInstance_Transform();
		ptr->Draw();
	}
}

void GUI_Editor_SkeletalMesh::Initialize()
{
	mMeshObject = MMeshManager::Get().Load<JSkeletalMeshObject>(mTitle);
}

void GUI_Editor_SkeletalMesh::Update(float DeltaTime)
{
	GUI_Editor_Base::Update(DeltaTime);
	if (mMeshObject)
	{
		mMeshObject->Tick(DeltaTime);
	}
}

void GUI_Editor_SkeletalMesh::Update_Implementation(float DeltaTime)
{
	GUI_Editor_Base::Update_Implementation(DeltaTime);

	DrawProperty();

	ImGui::SameLine();

	DrawViewport();

	ImGui::SameLine();

	DrawAnimationPreview();

	DrawMaterialSlot();

	DrawSaveButton();
}

void GUI_Editor_SkeletalMesh::DrawViewport() const
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

void GUI_Editor_SkeletalMesh::DrawProperty() const
{
	ImGui::SameLine();

	if (ImGui::BeginChild("Property",
						  ImVec2(mWindowSize.x * 0.33f, 0),
						  ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border))
	{
		if (mMeshObject)
		{
			// 원시 파일 경로 표시
			ImGui::Text("Mesh Name");
			ImGui::Text(&mMeshObject->mName[0]);

			// Bone Hierachy 표시
			ImGui::Text("Bone Hierachy");
			const auto& skeletal = std::dynamic_pointer_cast<JSkeletalMesh>(mMeshObject->mPrimitiveMeshData[0]);
			if (skeletal)
			{
				const auto&   jointData = skeletal->GetSkeletonData().Joints;
				const int32_t jointSize = jointData.size();
				int32_t       index     = 0;
				while (index < jointSize)
				{
					const FJointData& bone = jointData[index];
					ImGui::Text(bone.Name.c_str());

					index++;
				}
			}
		}
	}

	ImGui::EndChild();
}

void GUI_Editor_SkeletalMesh::DrawMaterialSlot() const
{
	if (mMeshObject)
	{
		ImGui::BeginGroup();

		ImGui::Text("Material Slot");

		const int32_t meshDataSize = mMeshObject->mPrimitiveMeshData.size();
		for (int32_t i = 0; i < meshDataSize; ++i)
		{
			auto& subMeshes = mMeshObject->mPrimitiveMeshData[i]->GetSubMesh();
			// auto  materialSlot = mMeshObject->mPrimitiveMeshData[i]->GetMaterialInstance();

			const int32_t subMeshSize = subMeshes.size();

			if (subMeshSize == 0)
			{

				ImGui::ImageButton("MaterialSlot", nullptr, ImVec2(100, 100));

				if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
				{
					const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
					const char*         str     = static_cast<const char*>(payload->Data);

					auto metaData = Utils::Serialization::GetType(str);

					if (metaData.AssetType == HASH_ASSET_TYPE_MATERIAL_INSTANCE)
					{
						// if (auto matInstancePtr = MMaterialInstanceManager::Get().CreateOrLoad(str))
						// {
						// 	mMeshObject->mPrimitiveMeshData[i]->SetMaterialInstance(matInstancePtr);
						// }
					}

				}

				ImGui::SameLine();
				// if (materialSlot)
				// {
				// 	ImGui::Text(materialSlot->GetMaterialName().c_str());
				// }
				// else
				// {
				// 	ImGui::Text("None Selected");
				// }


			}
			else
			{
				for (int32_t j = 0; j < subMeshSize; ++j)
				{

					ImGui::ImageButton(nullptr, ImVec2(100, 100));
					ImGui::Text(subMeshes[j]->GetName().c_str());

					if (ImGui::IsMouseReleased(0) && ImGui::IsItemHovered() && ImGui::BeginDragDropTarget())
					{
						const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
						const char*         str     = static_cast<const char*>(payload->Data);

						auto metaData = Utils::Serialization::GetType(str);

						// if (metaData.AssetType == HASH_ASSET_TYPE_MATERIAL_INSTANCE)
						// {
						// 	if (auto matInstancePtr = MMaterialInstanceManager::Get().CreateOrLoad(str))
						// 	{
						// 		subMeshes[j]->SetMaterialInstance(matInstancePtr);
						// 		ImGui::EndDragDropTarget();
						//
						// 	}
						// }
						// if (auto subMat = subMeshes[j]->GetMaterialInstance())
						// {
						// 	ImGui::Text(subMat->GetMaterialName().c_str());
						// }
						// else
						// {
						// 	ImGui::Text("NONE");
						// }
					}
				}
			}
		}
		ImGui::EndGroup();
	}

	ImGui::EndChild();
}

void GUI_Editor_SkeletalMesh::DrawAnimationPreview() const
{
	if (ImGui::BeginChild("RightView", ImVec2(0, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border))


		ImGui::ImageButton(nullptr, ImVec2(100, 100));
	// ImGui::Text(subMeshes[j]->GetName().c_str());

	if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
		const char*         str     = static_cast<const char*>(payload->Data);

		auto metaData = Utils::Serialization::GetType(str);

		if (metaData.AssetType == HASH_ASSET_TYPE_ANIMATION_CLIP)
		{
			// JAnimationClip* clip = MakePtr<JAnimationClip>();
			// if (!Utils::Serialization::DeSerialize(str, clip.get()))
			// {
			// 	LOG_CORE_ERROR("Failed to load animation object(Invalid Path maybe...): {0}",
			// 				   "Game/Animation/Unreal Take.jasset");
			// }
			//
			// mMeshObject->SetAnimation(clip);

			ImGui::EndDragDropTarget();
		}
	}
}

void GUI_Editor_SkeletalMesh::DrawSaveButton() const
{
	ImGui::Separator();

	if (ImGui::Button("Save"))
	{
		if (mMeshObject)
		{
			Utils::Serialization::Serialize(mTitle.c_str(), mMeshObject);
		}
	}
}

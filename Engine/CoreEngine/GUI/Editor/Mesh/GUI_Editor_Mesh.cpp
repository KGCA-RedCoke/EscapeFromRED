#include "GUI_Editor_Mesh.h"

#include "Core/Entity/Camera/JCamera.h"
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

	if (const auto& ptr = mMeshObject.lock())
	{
		ptr->Draw();
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

	DrawMaterialSlot();

	ImGui::Separator();

	if (ImGui::Button("Save"))
	{
		auto ptr = mMeshObject.lock();
		if (ptr)
		{
			Utils::Serialization::Serialize(mTitle.c_str(), ptr.get());
		}
	}
}

void GUI_Editor_Mesh::DrawViewport() const
{
	if (ImGui::BeginChild("ActorView", ImVec2(400, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border))
	{
		ImGui::Image(mViewport->SRV.Get(), ImGui::GetContentRegionAvail());

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
		{
			if (mCamera)
			{
				mCamera->Update(mDeltaTime);
			}
		}
		ImGui::EndChild();
	}
}

void GUI_Editor_Mesh::DrawMaterialSlot() const
{
	ImGui::SameLine();

	if (const auto& ptr = mMeshObject.lock())
	{
		ImGui::BeginGroup();

		ImGui::Text("Material Slot");

		const int32_t meshDataSize = ptr->mPrimitiveMeshData.size();
		for (int32_t i = 0; i < meshDataSize; ++i)
		{
			auto& subMeshes    = ptr->mPrimitiveMeshData[i]->GetSubMesh();
			auto  materialSlot = ptr->mPrimitiveMeshData[i]->GetMaterialInstance();

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
						if (auto matInstancePtr = MMaterialInstanceManager::Get().CreateOrLoad(str))
						{
							ptr->mPrimitiveMeshData[i]->SetMaterialInstance(matInstancePtr);
						}
					}

				}

				if (materialSlot)
				{
					ImGui::Text(materialSlot->GetMaterialName().c_str());
				}
				else
				{
					ImGui::Text("NONE");
				}


			}
			else
			{
				for (int32_t j = 0; j < subMeshSize; ++j)
				{
					ImGui::ImageButton("MaterialSlot", nullptr, ImVec2(100, 100));

					if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
					{
						const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
						const char*         str     = static_cast<const char*>(payload->Data);

						auto metaData = Utils::Serialization::GetType(str);

						if (metaData.AssetType == HASH_ASSET_TYPE_MATERIAL_INSTANCE)
						{
							if (auto matInstancePtr = MMaterialInstanceManager::Get().CreateOrLoad(str))
							{
								subMeshes[j]->SetMaterialInstance(matInstancePtr);
							}
						}
						if (materialSlot)
						{
							ImGui::Text(materialSlot->GetMaterialName().c_str());
						}
						else
						{
							ImGui::Text("NONE");
						}
					}
				}
			}
		}

		ImGui::EndGroup();
	}
}

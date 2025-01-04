#include "GUI_Editor_Material.h"

#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Graphics/Material/Instance/JMaterialInstance.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/Logger.h"
#include "GUI/Popup/GUI_Popup_FileBrowser.h"


GUI_Editor_Material::GUI_Editor_Material(const JText& InTitle)
	: GUI_Editor_Base(InTitle),
	  bOpenFileBrowser(false),
	  bOpenFolder(false)
{
	mMaterialToEdit = GetWorld.MaterialInstanceManager->Load(InTitle);

	SetMeshObject(Path_Mesh_Sphere);
}

void GUI_Editor_Material::Render()
{
	using Super = GUI_Editor_Base;

	Super::Render();

	// 변경사항 적용 Draw
	mPreviewMeshObject->UpdateInstance_Transform();
	mPreviewMeshObject->Draw();
}

void GUI_Editor_Material::SetMeshObject(JTextView InMeshPath)
{
	// 3. 구체 메시를 생성 (굳이 구체가 아니어도 상관없음, 대부분의 엔진에서는 구체를 사용)
	mPreviewMeshObject = GetWorld.MeshManager->Clone(InMeshPath.data());
	mPreviewMeshObject->SetMaterialInstance(mMaterialToEdit);
}

void GUI_Editor_Material::ShowMaterialEditor()
{
	ImGui::BeginChild("MaterialView", ImVec2(400, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border);

	ImGui::Image(mViewport->SRV.Get(), ImGui::GetContentRegionAvail());

	if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
	{
		mCamera->Tick(mDeltaTime);
	}

	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("MaterialEditor", ImVec2(ImGui::GetContentRegionAvail().x, 0), true);

	// Mesh Selector
	if (ImGui::BeginCombo("MeshSelector", mPreviewMeshObject->mName.c_str()))
	{
		const JArray<JMeshObject*>& loaded = GetWorld.MeshManager->GetManagedList();
		for (auto& loadedMesh : loaded)
		{
			JText meshName = loadedMesh->mName;
			if (ImGui::Selectable(meshName.c_str()))
			{
				SetMeshObject(meshName);
				break;
			}
		}

		ImGui::EndCombo();
	}

	if (ImGui::CollapsingHeader("Properties"))
	{
		ImGui::InputText("Name",
						 &mMaterialToEdit->mFileName[0],
						 256,
						 ImGuiInputTextFlags_CharsNoBlank);

		auto materialRef = mMaterialToEdit->mParentMaterial;

		ImGui::Text("Parent Material");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##ParentMaterial", materialRef ? materialRef->GetMaterialName().c_str() : "None Selected"))
		{
			const JArray<JMaterial*>& loaded = GetWorld.MaterialManager->GetManagedList();
			for (auto& loadedMaterial : loaded)
			{
				JText materialName = loadedMaterial->GetMaterialName();
				if (ImGui::Selectable(materialName.c_str()))
				{
					mMaterialToEdit->SetParentMaterial(loadedMaterial);
					break;
				}
			}

			ImGui::EndCombo();
		}
	}

	if (ImGui::CollapsingHeader("Shader Params"))
	{
		const auto& params = mMaterialToEdit->mParentMaterial->mMaterialParams;
		for (int32_t i = 0; i < params.size(); ++i)
		{
			const FMaterialParam& param = params[i];

			switch (param.ParamValue)
			{
			case EMaterialParamValue::Float:
				HandleFloatType(param);
				break;
			case EMaterialParamValue::Float2:
				HandleFloat2Type(param);
				break;
			case EMaterialParamValue::Float3:
			case EMaterialParamValue::Float4:
				HandleFloat4Type(param);
				break;
			case EMaterialParamValue::Texture2D:
				HandleTextureType(param);
				break;
			}
		}
	}

	ImGui::Separator();

	Button_AddParam();

	ImGui::Separator();

	if (ImGui::Button("Cancel"))
	{}

	ImGui::SameLine();

	if (ImGui::Button("Save"))
	{
		Utils::Serialization::Serialize(mMaterialToEdit->mFileName.c_str(), mMaterialToEdit);
	}

	ImGui::EndChild();
}

void GUI_Editor_Material::HandleFloatType(const FMaterialParam& Param)
{
	if (float* instanceValue = static_cast<float*>(mMaterialToEdit->GetInstanceParam(Param.Key)))
	{
		float cachedValue = *instanceValue;

		ImGui::DragFloat(Param.Name.c_str(), instanceValue, 0.01f, 0.f, 100.f);

		if (cachedValue != *instanceValue)
		{
			mMaterialToEdit->OnMaterialInstanceParamChanged.Execute();
		}
	}
}

void GUI_Editor_Material::HandleFloat2Type(const FMaterialParam& Param)
{
	if (float* instanceValue = static_cast<float*>(mMaterialToEdit->GetInstanceParam(Param.Key)))
	{
		FVector2 cachedValue = FVector2{instanceValue[0], instanceValue[1]};

		ImGui::DragFloat2(Param.Name.c_str(), instanceValue, 0.01f, 0.f, 1.f);

		if (cachedValue.x != instanceValue[0] || cachedValue.y != instanceValue[1])
		{
			mMaterialToEdit->OnMaterialInstanceParamChanged.Execute();
			// mMaterialToEdit->EditInstanceParam(Param.Name, Param);
		}
	}
}

void GUI_Editor_Material::HandleFloat3Type(const FMaterialParam& Param)
{
	if (float* instanceValue = static_cast<float*>(mMaterialToEdit->GetInstanceParam(Param.Key)))
	{
		FVector cachedValue = FVector{instanceValue[0], instanceValue[1], instanceValue[2]};

		ImGui::DragFloat3(Param.Name.c_str(), instanceValue, 0.01f, 0.f, 1.f);

		if (cachedValue != FVector{instanceValue[0], instanceValue[1], instanceValue[2]})
		{
			mMaterialToEdit->OnMaterialInstanceParamChanged.Execute();
		}
	}
}

void GUI_Editor_Material::HandleFloat4Type(const FMaterialParam& Param) const
{
	if (float* instanceValue = static_cast<float*>(mMaterialToEdit->GetInstanceParam(Param.Key)))
	{
		FVector4 cachedValue = FVector4{instanceValue[0], instanceValue[1], instanceValue[2], instanceValue[3]};

		ImGui::DragFloat4(Param.Name.c_str(), instanceValue, 0.01f, 0.f, 1.f);

		if (cachedValue != FVector4{instanceValue[0], instanceValue[1], instanceValue[2], instanceValue[3]})
		{
			mMaterialToEdit->OnMaterialInstanceParamChanged.Execute();
		}
	}
}


void GUI_Editor_Material::HandleTextureType(const FMaterialParam& Param) const
{
	ImGui::Text(Param.Name.c_str());
	ImGui::SameLine();

	JText label = std::format("##{}", Param.Name);

	FMaterialParam textureParam;
	textureParam.Name       = Param.Name;
	textureParam.Key        = Param.Key;
	textureParam.ParamValue = EMaterialParamValue::Texture2D;

	JTexture* instanceValue = static_cast<JTexture*>(mMaterialToEdit->GetInstanceParam(Param.Key, true));

	// Texture Path
	if (ImGui::BeginCombo(label.c_str(), Param.StringValue.c_str()))
	{
		std::vector<JTexture*> loaded = GetWorld.TextureManager->GetManagedList();

		for (const auto& tex : loaded)
		{
			JText texName = WString2String(tex->GetTextureName());
			if (ImGui::Selectable(texName.c_str()))
			{
				textureParam.StringValue  = texName;
				textureParam.TextureValue = tex;

				mMaterialToEdit->EditInstanceParam(Param.Name, textureParam);

				break;
			}
		}


		ImGui::EndCombo();
	}

	ImGui::Text(u8("텍스처"));
	ImGui::SameLine();
	ImGui::Image(instanceValue ? instanceValue->GetSRV() : GetWorld.TextureManager->WhiteTexture->GetSRV(), {100, 100});

	if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();

		const char* assetPath = static_cast<const char*>(payload->Data);

		textureParam.StringValue  = assetPath;
		textureParam.TextureValue = GetWorld.TextureManager->Load(assetPath);
		mMaterialToEdit->EditInstanceParam(Param.Name, textureParam);

	}
}


void GUI_Editor_Material::Button_AddParam()
{
	// 파라미터 추가 버튼
	if (ImGui::Button("New Param"))
	{
		bOpenNewParamPopup = true;
	}
}

void GUI_Editor_Material::Update_Implementation(float DeltaTime)
{
	mDeltaTime = DeltaTime;

	ShowMaterialEditor();

	if (bOpenNewParamPopup)
	{
		ImGui::OpenPopup("NewParamPopup");
		if (ImGui::BeginPopup("NewParamPopup"))
		{
			// Name Slot
			static char paramName[256];
			ImGui::InputText("Param Name", paramName, 256);

			static uint8_t selectedType = 0; // 기본값을 원하는 초기 값으로 설정

			// Type Slot
			if (ImGui::BeginMenu("ParamType"))
			{
				for (uint8_t i = 0; i < static_cast<uint8_t>(EMaterialParamValue::Max); ++i)
				{
					if (ImGui::MenuItem(HASH_MATERIAL_PARAM_VALUE_TYPE[i], nullptr, selectedType == i))
					{
						int32_t customParamCount = 0;
						for (auto& param : mMaterialToEdit->mParentMaterial->mMaterialParams)
						{
							if (param.Name.starts_with("Custom"))
							{
								customParamCount++;
							}
						}

						selectedType = i; // 선택된 타입 업데이트
						mMaterialToEdit->mParentMaterial->mMaterialParams.push_back(
							 FMaterialParam(
											std::format("CustomParam_{}", customParamCount),
											static_cast<EMaterialParamValue>(selectedType),
											&FVector4::ZeroVector,
											true
										   ));

						bOpenNewParamPopup = false;
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();

		}
	}
}

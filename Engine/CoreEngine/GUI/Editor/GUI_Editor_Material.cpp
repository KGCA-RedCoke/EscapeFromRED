#include "GUI_Editor_Material.h"
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Logger.h"
#include "GUI/Popup/GUI_Popup_FileBrowser.h"


GUI_Editor_Material::GUI_Editor_Material(const JText& InTitle)
	: GUI_Base(InTitle),
	  bOpenFileBrowser(false),
	  bShowNewParam(false),
	  bOpenFolder(false),
	  mFilePath{"Game/Materials"},
	  mDeltaTime(0)
{
	mWindowFlags |= ImGuiWindowFlags_MenuBar;
	// s_MaterialEditorNum++;
	// s_CachedMaterialEditorIndex = s_MaterialEditorNum;

	// 1. 뷰포트를 생성
	mViewport = IManager.ViewportManager->CreateOrLoad(mTitle, 1280, 720);
	assert(mViewport);

	// 2. 머티리얼 에디터 프리뷰에 사용될 카메라를 생성
	mCamera = IManager.CameraManager->CreateOrLoad(InTitle);
	assert(mCamera);

	// 3. 구체 메시를 생성 (굳이 구체가 아니어도 상관없음, 대부분의 엔진에서는 구체를 사용)
	mPreviewMeshObject = IManager.MeshManager->CreateOrClone("Cube");
	assert(mPreviewMeshObject);
	// 3.1 구체의 머티리얼 데이터를 가져온다. (구체는 서브메시가 없으므로 첫번째 메시를 가져온다.)
	mMaterialToEdit = IManager.MaterialInstanceManager->CreateOrLoad<
		JMaterialInstance>("Game/Materials/NewMaterial.jasset");
	// mMaterialToEdit = mPreviewMeshObject->mPrimitiveMeshData[0]->mMaterial;
	assert(mMaterialToEdit);

	mPreviewMeshObject->mPrimitiveMeshData[0]->mMaterialInstance = mMaterialToEdit;
	// 3.2 머티리얼의 셰이더 데이터를 가져온다.
	// mShader = mMaterialToEdit->GetShader();
	// assert(mShader);
}

GUI_Editor_Material::~GUI_Editor_Material()
{
	// s_MaterialEditorNum--;
	// s_CachedMaterialEditorIndex = s_MaterialEditorNum;
}

void GUI_Editor_Material::Render()
{
	// 렌더 타겟을 현재 뷰포트로 설정
	IManager.ViewportManager->SetRenderTarget(mTitle.c_str());

	// 구체는 원점에 유지시키고 카메라 위치만 업데이트 시켜준다.
	// mPreviewMeshObject->UpdateBuffer(FMatrix::Identity, mCamera);

	// 변경사항 적용 Draw
	mPreviewMeshObject->Render();
}

void GUI_Editor_Material::ShowMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Material"))
			{
				bOpenFileBrowser = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void GUI_Editor_Material::ShowMaterialEditor()
{
	ImGui::BeginChild("MaterialView", ImVec2(ImGui::GetContentRegionAvail().x * 0.6, 0), true);

	ImGui::Image(mViewport->SRV.Get(), ImGui::GetContentRegionAvail());

	if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
	{
		mCamera->Update(mDeltaTime);
	}

	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("MaterialEditor", ImVec2(ImGui::GetContentRegionAvail().x, 0), true);

	if (ImGui::CollapsingHeader("Properties"))
	{
		ImGui::InputText("Name",
						 &mMaterialToEdit->mFileName[0],
						 256,
						 ImGuiInputTextFlags_CharsNoBlank);

		// Ptr<JShader> shader         = mMaterialToEdit->mParentMaterial->GetShader();
		// JText        shaderFileName = "None Selected";
		// if (shader)
		// {
		// 	shaderFileName = WString2String(shader->GetShaderFile());
		// }

		// ImGui::Text("Shader");
		// ImGui::SameLine();
		// if (ImGui::BeginCombo("##ShaderModel", shaderFileName.c_str()))
		// {
		// 	const std::vector<Ptr<JShader>> loaded = IManager.ShaderManager->GetManagedList();
		// 	for (auto& loadedShader : loaded)
		// 	{
		// 		JText shaderName = WString2String(loadedShader->GetShaderFile());
		// 		if (ImGui::Selectable(shaderName.c_str()))
		// 		{
		// 			mMaterialToEdit->SetShader(loadedShader);
		// 			mShader = loadedShader;
		// 		}
		// 	}
		// 	ImGui::EndCombo();
		// }
	}


	if (ImGui::CollapsingHeader("Shader Params"))
	{
		auto& params = mMaterialToEdit->mInstanceParams;
		for (int32_t i = 0; i < params.size(); ++i)
		{
			FMaterialParam& param = params[i];
			float           color[3];

	
			switch (param.ParamValue)
			{
			case EMaterialParamValue::Float:
				ImGui::DragFloat(param.Name.c_str(), &param.FloatValue, 0.01f, 0.f, 1.f);

				break;

			case EMaterialParamValue::Float3:
				// ShowTextureSlot(mMaterialToEdit, param.Name.c_str());

				ImGui::DragFloat3(param.Name.c_str(), &param.Float3Value.x, 0.01f, 0.f, 1.f);
				color[0] = param.Float4Value.x;
				color[1] = param.Float4Value.y;
				color[2] = param.Float4Value.z;
				ImGui::SameLine();
				ImGui::ColorEdit3("MyColor##3",
								  (float*)&color,
								  ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

				param.Float3Value = FVector{color[0], color[1], color[2]};
				// mMaterialToEdit->UpdateMaterialBuffer(IManager.RenderManager->GetImmediateDeviceContext());
				break;
			}
		}
	}

	ImGui::Separator();

	if (ImGui::Button("##Add New Param"))
	{
		bShowNewParam = true;
	}

	ImGui::InputText("##SavePath", mFilePath, 256);
	ImGui::SameLine();
	if (ImGui::Button("Open"))
	{
		bOpenFileBrowser = true;
		bOpenFolder      = true;
	}

	if (ImGui::Button("Cancel"))
	{}

	ImGui::SameLine();

	if (ImGui::Button("Save"))
	{
		Utils::Serialization::Serialize(mMaterialToEdit->mFileName.c_str(), mMaterialToEdit.get());
	}

	ImGui::EndChild();
}

void GUI_Editor_Material::ShowFileBrowser()
{
	ImGui::FileBrowserOption option = bOpenFolder ? ImGui::FileBrowserOption::DIRECTORY : ImGui::FileBrowserOption::FILE;
	std::set<JText>          extent = {".jasset"};

	if (ImGui::OpenFileBrowser(mFilePath, option, extent))
	{
		JText filePath;
		if (ImGui::FetchFileBrowserResult(mFilePath, filePath))
		{
			Utils::Serialization::DeSerialize(filePath.c_str(), mMaterialToEdit.get());
			bOpenFileBrowser = false;
			bOpenFolder      = false;
		}
	}
}

void GUI_Editor_Material::ShowTextureSlot(const Ptr<JMaterialInstance>& InMaterialData, const char* TextureType) const
{
	// ImGui::Text(TextureType);
	// ImGui::SameLine();
	//
	// // Texture Thumbnail
	// FMaterialParam* params = InMaterialData->GetMaterialParam(TextureType);
	// if (!params)
	// {
	// 	FMaterialParam newParams;
	// 	newParams.Name       = TextureType;
	// 	newParams.ParamValue = EMaterialParamValue::Texture2D;
	//
	// 	InMaterialData->AddMaterialParam(newParams);
	// 	params = InMaterialData->GetMaterialParam(TextureType);
	// }
	//
	// if (!(!params->StringValue.empty() && params->TextureValue))
	// {
	//
	// 	params->TextureValue = IManager.TextureManager->CreateOrLoad("rsc/Icons/NoIcon.png");
	// 	params->StringValue  = "rsc/Icons/NoIcon.png";
	// }
	//
	// JText label = std::format("##{}", TextureType);
	//
	// // Texture Path
	// if (ImGui::BeginCombo(label.c_str(), params->StringValue.c_str()))
	// {
	// 	std::vector<Ptr<JTexture>> loaded = IManager.TextureManager->GetManagedList();
	//
	// 	for (auto& tex : loaded)
	// 	{
	// 		JText texName = WString2String(tex->GetPath());
	// 		if (ImGui::Selectable(texName.c_str()))
	// 		{
	// 			params->ParamValue   = EMaterialParamValue::Texture2D;
	// 			params->StringValue  = texName;
	// 			params->TextureValue = tex;
	// 		}
	// 	}
	//
	// 	ImGui::EndCombo();
	// }
	//
	//
	// assert(params->TextureValue);
	// ImGui::Image(params->TextureValue->GetSRV(), ImVec2(64, 64));

}


void GUI_Editor_Material::Update_Implementation(float DeltaTime)
{
	mDeltaTime = DeltaTime;

	ShowMaterialEditor();

	if (bOpenFileBrowser)
	{
		ShowFileBrowser();
	}

	if (bShowNewParam)
	{
		// ShowParamPopup();
	}
}

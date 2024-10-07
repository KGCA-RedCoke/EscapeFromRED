#include "GUI_Editor_Material.h"
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Logger.h"
#include "GUI/Popup/GUI_Popup_FileBrowser.h"


GUI_Editor_Material::GUI_Editor_Material(const JText& InTitle)
	: GUI_Base(InTitle),
	  bOpenFileBrowser(false),
	  mDeltaTime(0)
{
	mWindowFlags |= ImGuiWindowFlags_MenuBar;
	s_MaterialEditorNum++;
	s_CachedMaterialEditorIndex = s_MaterialEditorNum;

	// 1. 뷰포트를 생성
	mViewport = IManager.ViewportManager->CreateOrLoad(mTitle, 1280, 720);
	assert(mViewport);

	// 2. 머티리얼 에디터 프리뷰에 사용될 카메라를 생성
	mCamera = IManager.CameraManager->CreateOrLoad(InTitle);
	assert(mCamera);

	// 3. 구체 메시를 생성 (굳이 구체가 아니어도 상관없음, 대부분의 엔진에서는 구체를 사용)
	mSphere = IManager.MeshManager->CreateOrLoad("Game/Mesh/SM_CP_Mid_Male_Body.jasset");
	assert(mSphere);
	// 3.1 구체의 머티리얼 데이터를 가져온다. (구체는 서브메시가 없으므로 첫번째 메시를 가져온다.)
	mMaterialToEdit = mSphere->mPrimitiveMeshData[0]->mMaterial;
	assert(mMaterialToEdit);
	// 3.2 머티리얼의 셰이더 데이터를 가져온다.
	mShader = mMaterialToEdit->GetShader();
	assert(mShader);
}

GUI_Editor_Material::~GUI_Editor_Material()
{
	s_MaterialEditorNum--;
	s_CachedMaterialEditorIndex = s_MaterialEditorNum;
}

void GUI_Editor_Material::Render()
{
	// 렌더 타겟을 현재 뷰포트로 설정
	IManager.ViewportManager->SetRenderTarget(mTitle.c_str());

	// 구체는 원점에 유지시키고 카메라 위치만 업데이트 시켜준다.
	mSphere->UpdateBuffer(FMatrix::Identity, mCamera);

	// 변경사항 적용 Draw
	mSphere->Render();
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
						 &mMaterialToEdit->mMaterialName[0],
						 256,
						 ImGuiInputTextFlags_CharsNoBlank);

		Ptr<JShader> shader         = mMaterialToEdit->GetShader();
		JText        shaderFileName = "None Selected";
		if (shader)
		{
			shaderFileName = WString2String(shader->GetShaderFile());
		}

		ImGui::Text("Shader");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##ShaderModel", shaderFileName.c_str()))
		{
			const std::vector<Ptr<JShader>> loaded = IManager.ShaderManager->GetManagedList();
			for (auto& loadedShader : loaded)
			{
				JText shaderName = WString2String(loadedShader->GetShaderFile());
				if (ImGui::Selectable(shaderName.c_str()))
				{
					mMaterialToEdit->SetShader(loadedShader);
					mShader = loadedShader;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Checkbox("Transparent", &mMaterialToEdit->bTransparent);
	}

	if (ImGui::CollapsingHeader("Shader Params"))
	{
		auto& params = mMaterialToEdit->GetMaterialParams();
		for (int32_t i = 0; i < params.size(); ++i)
		{
			FMaterialParams& param = params[i];
			float            color[3];

			switch (param.Flags)
			{
			case EMaterialFlag::DiffuseColor:
			case EMaterialFlag::EmissiveColor:
			case EMaterialFlag::SpecularColor:
			case EMaterialFlag::ReflectionColor:
			case EMaterialFlag::AmbientColor:
			case EMaterialFlag::NormalMap:
			case EMaterialFlag::DisplacementColor:
			case EMaterialFlag::TransparentColor:
				ShowTextureSlot(mMaterialToEdit, param.Flags, param.Name.c_str());

				ImGui::DragFloat3(param.Name.c_str(), &param.Float3Value.x, 0.01f, 0.f, 1.f);
				color[0] = param.Float4Value.x;
				color[1] = param.Float4Value.y;
				color[2] = param.Float4Value.z;
				ImGui::SameLine();
				ImGui::ColorEdit3("MyColor##3",
								  (float*)&color,
								  ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

				param.Float3Value = FVector{color[0], color[1], color[2]};
				break;
			case EMaterialFlag::DiffuseFactor:
			case EMaterialFlag::EmissiveFactor:
			case EMaterialFlag::SpecularFactor:
			case EMaterialFlag::ReflectionFactor:
			case EMaterialFlag::AmbientFactor:
			case EMaterialFlag::TransparentFactor:
			case EMaterialFlag::DisplacementFactor:
			case EMaterialFlag::Shininess:
			case EMaterialFlag::Opacity:
				ImGui::DragFloat(param.Name.c_str(), &param.FloatValue, 0.01f, 0.f, 1.f);
				break;
			}

		}
	}

	ImGui::Separator();

	// if (ImGui::CollapsingHeader("Textures"))
	// {
	// 	// 구체는 메시가 하나만 있으므로 0번째 메시를 가져온다.
	// 	JMeshData* meshData     = mSphere->mPrimitiveMeshData[0].get();
	// 	JMaterial* materialData = meshData->mMaterial;
	//
	// 	// Texture Params
	// 	if (!materialData)
	// 	{
	// 		materialData = mMaterialToEdit;
	// 	}
	// 	ShowTextureSlot(materialData, TODO, NAME_MAT_Diffuse);
	// 	ShowTextureSlot(materialData, TODO, NAME_MAT_Normal);
	// }

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
		auto fileName = std::format("Game/Materials/{}.jasset", mMaterialToEdit->mMaterialName.c_str());
		Utils::Serialization::Serialize(mMaterialToEdit->mMaterialName.c_str(), mMaterialToEdit.get());

		// mMaterialToEdit = nullptr;

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
			// Utils::Serialization::DeSerialize(filePath.c_str(), mMaterialToEdit.get());
			bOpenFileBrowser = false;
			bOpenFolder      = false;
		}
	}
}

void GUI_Editor_Material::ShowTextureSlot(const Ptr<JMaterial>& InMaterialData, EMaterialFlag Flags,
										  const char*           TextureType) const
{
	ImGui::Text(TextureType);
	ImGui::SameLine();

	// Texture Thumbnail
	FMaterialParams* params = InMaterialData->GetMaterialParam(TextureType);
	if (!params)
	{
		FMaterialParams newParams;
		newParams.Name      = TextureType;
		newParams.Key       = StringHash(TextureType);
		newParams.ParamType = EMaterialParamType::Texture2D;

		InMaterialData->AddMaterialParam(newParams);
		params = InMaterialData->GetMaterialParam(TextureType);
	}

	if (!(!params->StringValue.empty() && params->TextureValue))
	{

		params->TextureValue = IManager.TextureManager->CreateOrLoad("rsc/Icons/NoIcon.png");
		params->StringValue  = "rsc/Icons/NoIcon.png";
	}

	JText label = std::format("##{}", TextureType);

	// Texture Path
	if (ImGui::BeginCombo(label.c_str(), params->StringValue.c_str()))
	{
		std::vector<Ptr<JTexture>> loaded = IManager.TextureManager->GetManagedList();

		for (auto& tex : loaded)
		{
			JText texName = WString2String(tex->GetPath());
			if (ImGui::Selectable(texName.c_str()))
			{
				params->Flags        = Flags;
				params->ParamType    = EMaterialParamType::Texture2D;
				params->StringValue  = texName;
				params->TextureValue = tex;
			}
		}

		ImGui::EndCombo();
	}


	assert(params->TextureValue);
	ImGui::Image(params->TextureValue->GetSRV(), ImVec2(64, 64));

}


void GUI_Editor_Material::Update_Implementation(float DeltaTime)
{
	mDeltaTime = DeltaTime;

	ShowMaterialEditor();

	if (bOpenFileBrowser)
	{
		ShowFileBrowser();
	}
}

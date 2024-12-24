#include "MMaterialManager.h"

#include "JMaterial_2D.h"
#include "JMaterial_Basic.h"
#include "JMaterial_Detail.h"
#include "JMaterial_POM.h"
#include "JMaterial_SkySphere.h"

void MMaterialManager::SaveEngineMaterials()
{
	// Ptr<JMaterial_Basic> basicMat = CreateOrLoad<JMaterial_Basic>(NAME_MAT_BASIC);
	// assert(basicMat != nullptr);
	//
	// Utils::Serialization::Serialize(NAME_MAT_BASIC, basicMat.get());

	// Ptr<JMaterial> skySphere = CreateOrLoad<JMaterial_SkySphere>("SkySphere");
	// skySphere->SetShader(GetWorld.ShaderManager->CreateOrLoad("Shader/SkySphere.hlsl"));
	//
	// assert(skySphere != nullptr);
	//
	// Utils::Serialization::Serialize("Game/Materials/Engine/Material_SkySphere.jasset", skySphere.get());

	// auto mirror = CreateOrLoad<JMaterial_Window>(NAME_MAT_WINDOW);
	// mirror->SetShader(MShaderManager::Get().CreateOrLoad("Shader/Mirror.hlsl"));
	// Utils::Serialization::Serialize(NAME_MAT_WINDOW, mirror);

	// auto ui = Load<JMaterial_2D>(NAME_MAT_2D);
	// ui->SetShader(MShaderManager::Get().UIShader);
	// Utils::Serialization::Serialize(NAME_MAT_2D, ui);

	// auto POM = Load<JMaterial_POM>(NAME_MAT_POM);
	// POM->SetShader(MShaderManager::Get().Load("Shader/HorrorMap_POM.hlsl"));
	// auto detail = Load<JMaterial_Detail>(NAME_MAT_DETAIL);
	// detail->SetShader(MShaderManager::Get().Load("Shader/HorrorMap_Detail.hlsl"));
	//
	// Utils::Serialization::Serialize(NAME_MAT_POM, POM);
	// Utils::Serialization::Serialize(NAME_MAT_DETAIL, detail);

}

void MMaterialManager::LoadEngineMaterials()
{
	Load<JMaterial_Basic>(NAME_MAT_BASIC);
	Load<JMaterial_SkySphere>(NAME_MAT_SKYSPHERE);
	Load<JMaterial_2D>(NAME_MAT_2D);
	Load<JMaterial_POM>(NAME_MAT_POM);
	Load<JMaterial_Detail>(NAME_MAT_DETAIL);
}

MMaterialManager::MMaterialManager()
{
	LoadEngineMaterials();
}

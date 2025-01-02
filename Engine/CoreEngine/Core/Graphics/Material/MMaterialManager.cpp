#include "MMaterialManager.h"

#include "JMaterial_2D.h"
#include "JMaterial_Basic.h"
#include "JMaterial_Character.h"
#include "JMaterial_Detail.h"
#include "JMaterial_POM.h"
#include "JMaterial_SkySphere.h"
#include "JMaterial_Wind.h"
#include "Core/Graphics/Shader/JMaterial_Mirror.h"
#include "Core/Graphics/Shader/MShaderManager.h"

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

	auto mirror = Load<JMaterial_Mirror>(NAME_MAT_MIRROR);
	mirror->SetShader(MShaderManager::Get().Load("Shader/Mirror.hlsl"));
	Utils::Serialization::Serialize(NAME_MAT_MIRROR, mirror);

	auto wind = Load<JMaterial_Wind>(NAME_MAT_WIND);
	wind->SetShader(MShaderManager::Get().Load("Shader/Wind.hlsl"));
	Utils::Serialization::Serialize(NAME_MAT_WIND, wind);
	
	auto character = Load<JMaterial_Character>(NAME_MAT_CHARACTER);
	character->SetShader(MShaderManager::Get().Load("Shader/Character.hlsl"));
	Utils::Serialization::Serialize(NAME_MAT_CHARACTER, character);

}

void MMaterialManager::LoadEngineMaterials()
{
	Load<JMaterial_Basic>(NAME_MAT_BASIC);
	Load<JMaterial_SkySphere>(NAME_MAT_SKYSPHERE);
	Load<JMaterial_2D>(NAME_MAT_2D);
	Load<JMaterial_POM>(NAME_MAT_POM);
	Load<JMaterial_Detail>(NAME_MAT_DETAIL);
	Load<JMaterial_Mirror>(NAME_MAT_MIRROR);
	Load<JMaterial_Wind>(NAME_MAT_WIND);
	Load<JMaterial_Character>(NAME_MAT_CHARACTER);
}

MMaterialManager::MMaterialManager()
{
	LoadEngineMaterials();
}

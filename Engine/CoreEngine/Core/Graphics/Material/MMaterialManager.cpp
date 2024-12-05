#include "MMaterialManager.h"

#include "JMaterial_Basic.h"
#include "JMaterial_SkySphere.h"
#include "JMaterial_Window.h"
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
}

void MMaterialManager::LoadEngineMaterials()
{
	Load<JMaterial_Basic>(NAME_MAT_BASIC);
	Load<JMaterial_SkySphere>(NAME_MAT_SKYSPHERE);
	Load<JMaterial_Window>(NAME_MAT_WINDOW);
}	

MMaterialManager::MMaterialManager()
{
	LoadEngineMaterials();
}

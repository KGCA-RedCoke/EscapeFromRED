#include "MMaterialManager.h"

#include "JMaterial_Basic.h"
#include "JMaterial_SkySphere.h"
#include "Core/Interface/MManagerInterface.h"

void MMaterialManager::SaveEngineMaterials()
{
	// Ptr<JMaterial_Basic> basicMat = CreateOrLoad<JMaterial_Basic>(NAME_MAT_BASIC);
	// assert(basicMat != nullptr);
	//
	// Utils::Serialization::Serialize(NAME_MAT_BASIC, basicMat.get());
	Ptr<JMaterial> skySphere = CreateOrLoad<JMaterial_SkySphere>("SkySphere");
	skySphere->SetShader(IManager.ShaderManager->CreateOrLoad("Shader/SkySphere.hlsl"));
	
	assert(skySphere != nullptr);

	Utils::Serialization::Serialize("SkySphere", skySphere.get());
}

void MMaterialManager::LoadEngineMaterials()
{
	CreateOrLoad<JMaterial_Basic>(NAME_MAT_BASIC);
}

MMaterialManager::MMaterialManager()
{
	LoadEngineMaterials();
}

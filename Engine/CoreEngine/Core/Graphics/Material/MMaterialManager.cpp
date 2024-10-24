#include "MMaterialManager.h"

#include "JMaterial_Basic.h"
#include "Core/Interface/MManagerInterface.h"

void MMaterialManager::SaveEngineMaterials()
{
	Ptr<JMaterial_Basic> basicMat = CreateOrLoad<JMaterial_Basic>(NAME_MAT_BASIC);
	assert(basicMat != nullptr);

	Utils::Serialization::Serialize(NAME_MAT_BASIC, basicMat.get());
}

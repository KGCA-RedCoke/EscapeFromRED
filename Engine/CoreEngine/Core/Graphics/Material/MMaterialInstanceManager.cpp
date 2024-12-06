#include "MMaterialInstanceManager.h"

#include "MMaterialManager.h"

void MMaterialInstanceManager::PostInitialize(const JText&   OriginalNameOrPath, const JText& ParsedName,
											  const uint32_t NameHash, void*                  Entity)
{
	if (Utils::Serialization::IsJAssetFileAndExist(OriginalNameOrPath.c_str()))
	{
		Utils::Serialization::DeSerialize(OriginalNameOrPath.c_str(), static_cast<JMaterialInstance*>(Entity));
	}
	else
	{
		mManagedList[NameHash]->SetParentMaterial(MMaterialManager::Get().Load(NAME_MAT_BASIC));
	}
}

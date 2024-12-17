#include "MAnimataionManager.h"

void MAnimataionManager::PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
										void*        Entity)
{
	if (std::filesystem::is_regular_file(OriginalNameOrPath))
	{
		Utils::Serialization::DeSerialize(OriginalNameOrPath.data(), static_cast<ISerializable*>(Entity));
	}
}

MAnimataionManager::MAnimataionManager() {}
MAnimataionManager::~MAnimataionManager() {}

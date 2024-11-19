#pragma once
#include "Instance/JMaterialInstance.h"

class MMaterialInstanceManager : public Manager_Base<JMaterialInstance, MMaterialInstanceManager>
{
	void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash, void* Entity) override;
public:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MMaterialInstanceManager>;
	friend class JWorld;

	MMaterialInstanceManager()  = default;
	~MMaterialInstanceManager() = default;

public:
	MMaterialInstanceManager(const MMaterialInstanceManager&)            = delete;
	MMaterialInstanceManager& operator=(const MMaterialInstanceManager&) = delete;

#pragma endregion
};

#pragma once
#include "Instance/JMaterialInstance.h"

class MMaterialInstanceManager : public Manager_Base<JMaterialInstance, MMaterialInstanceManager>
{
public:
public:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MMaterialInstanceManager>;
	friend class MManagerInterface;

	MMaterialInstanceManager()  = default;
	~MMaterialInstanceManager() = default;

public:
	MMaterialInstanceManager(const MMaterialInstanceManager&)            = delete;
	MMaterialInstanceManager& operator=(const MMaterialInstanceManager&) = delete;

#pragma endregion
};

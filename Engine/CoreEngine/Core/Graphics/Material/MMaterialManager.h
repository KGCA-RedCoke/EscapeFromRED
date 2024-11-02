#pragma once
#include "JMaterial.h"
#include "Core/Manager/Manager_Base.h"

class MMaterialManager : public Manager_Base<JMaterial, MMaterialManager>
{
public:

	void SaveEngineMaterials();
	void LoadEngineMaterials();

public:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MMaterialManager>;
	friend class MManagerInterface;

	MMaterialManager();
	~MMaterialManager() = default;

public:
	MMaterialManager(const MMaterialManager&)            = delete;
	MMaterialManager& operator=(const MMaterialManager&) = delete;

#pragma endregion
};

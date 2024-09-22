#pragma once
#include "JMaterial.h"
#include "Core/Manager/Manager_Base.h"

class MMaterialManager : public Manager_Base<JMaterial, MMaterialManager>
{
public:
	void Initialize();

public:
	[[nodiscard]] FORCEINLINE JMaterial* GetDefaultMaterial() const { return DefaultMaterial; }

private:
	JMaterial* DefaultMaterial;

public:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MMaterialManager>;
	friend class MManagerInterface;

	MMaterialManager()  = default;
	~MMaterialManager() = default;

public:
	MMaterialManager(const MMaterialManager&)            = delete;
	MMaterialManager& operator=(const MMaterialManager&) = delete;

#pragma endregion
};

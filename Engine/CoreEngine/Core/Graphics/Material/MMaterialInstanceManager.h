#pragma once
#include "Instance/JMaterialInstance.h"

class MMaterialInstanceManager : public Manager_Base<JMaterialInstance, MMaterialInstanceManager>
{
public:
	void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
						void*        Entity) override;
	void UpdateMaterialInstance(const JMaterialInstance* InMaterialInstance);

public:
	JHash<JMaterial*, ComPtr<ID3D11Buffer>>             mBufferList;
	JHash<JMaterial*, ComPtr<ID3D11ShaderResourceView>> mSRVList;

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

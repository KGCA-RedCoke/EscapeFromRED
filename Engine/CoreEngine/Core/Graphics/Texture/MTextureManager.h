#pragma once
#include "JTexture.h"
#include "Core/Manager/Manager_Base.h"

#define Manager_Texture MTextureManager::Get()


class MTextureManager : public Manager_Base<JTexture, MTextureManager>
{

public:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MTextureManager>;
	friend class JWorld;

	MTextureManager()  = default;
	~MTextureManager() = default;

public:
	MTextureManager(const MTextureManager&)            = delete;
	MTextureManager& operator=(const MTextureManager&) = delete;

#pragma endregion
};

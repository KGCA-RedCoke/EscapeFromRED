#pragma once
#include "JTexture.h"
#include "Core/Manager/Manager_Base.h"

#define Manager_Texture MTextureManager::Get()

constexpr const char* DefaultAlbedoTexturePath = "rsc/Engine/Material/Default/Default_Albedo.jpg";
constexpr const char* DefaultNormalTexturePath = "rsc/Engine/Material/Default/Default_Normal.jpg";

inline const JTexture* DefaultAlbedoTexture = nullptr;
inline const JTexture* DefaultNormalTexture = nullptr;

class MTextureManager : public Manager_Base<JTexture, MTextureManager>
{
public:
	void Initialize();

public:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MTextureManager>;
	friend class MManagerInterface;

	MTextureManager()  = default;
	~MTextureManager() = default;

public:
	MTextureManager(const MTextureManager&)            = delete;
	MTextureManager& operator=(const MTextureManager&) = delete;

#pragma endregion
};

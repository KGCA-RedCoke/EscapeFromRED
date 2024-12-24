#pragma once
#include "JAnimationClip.h"
#include "JAnimator.h"
#include "Core/Manager/Manager_Base.h"

class MAnimatorManager : public Manager_Base<JAnimator, MAnimatorManager>
{
public:
	void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
						void*        Entity) override;

private:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MAnimatorManager>;

	MAnimatorManager();
	~MAnimatorManager();

public:
	MAnimatorManager(const MAnimatorManager&)            = delete;
	MAnimatorManager& operator=(const MAnimatorManager&) = delete;

#pragma endregion
};


class MAnimManager : public Manager_Base<JAnimationClip, MAnimManager>
{
public:
	void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
						void*        Entity) override;

	void BindAnimationTexture(ID3D11DeviceContext* InContext, JSkeletalMesh* InSkeletalMesh);

public:
	JHash<JText, uint32_t>                                  mAnimOffset;
	JHash<JSkeletalMesh*, JArray<FVector4>>                 mAnimTextures_Skeletal;
	JHash<JSkeletalMesh*, ComPtr<ID3D11Buffer>>             mAnimTextureBuffer;
	JHash<JSkeletalMesh*, ComPtr<ID3D11ShaderResourceView>> mAnimTextureBuffer_SRV;

private:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MAnimManager>;

	MAnimManager();
	~MAnimManager();

public:
	MAnimManager(const MAnimManager&)            = delete;
	MAnimManager& operator=(const MAnimManager&) = delete;

#pragma endregion
};

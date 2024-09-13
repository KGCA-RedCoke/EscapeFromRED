#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/FileIO/JSerialization.h"


/**
 * 텍스처 관련 리소스 집합
 * IManager->TextureManager로 관리된다.
 */
class JTexture : public ISerializable
{
public:
	JTexture();
	explicit JTexture(JWTextView InName);
	explicit JTexture(JTextView InName);
	~JTexture() = default;

public:
	void Serialize(std::ofstream& FileStream) override;
	void DeSerialize(std::ifstream& InFileStream) override;

public:
	void PreRender(int32_t InSlot = 0);

public:
	[[nodiscard]] FORCEINLINE JWText                          GetPath() const { return mTextureName; }
	[[nodiscard]] FORCEINLINE ID3D11ShaderResourceView*       GetSRV() const { return mShaderResourceView.Get(); }
	[[nodiscard]] FORCEINLINE D3D11_SHADER_RESOURCE_VIEW_DESC GetSrvDesc() const { return mSRVDesc; }
	[[nodiscard]] FORCEINLINE D3D11_TEXTURE2D_DESC            GetTextureDesc() const { return mTextureDesc; }

	/** 셰이더의 특정 슬롯에 이 텍스처를 할당하려면 이 메서드를 호출 (default slot = 0) */
	FORCEINLINE void SetShaderSlot(const uint32_t InSlotIndex) { mSlot = InSlotIndex; }

private:
	void LoadFromFile();

private:
	uint32_t mSlot;
	uint32_t mID /** Hashing된 Key값 */;
	JWText   mTextureName /** 실제 텍스처 경로 */;

	ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
	D3D11_SHADER_RESOURCE_VIEW_DESC  mSRVDesc;
	D3D11_TEXTURE2D_DESC             mTextureDesc; // 크기, 형식, 샘플링 등
};

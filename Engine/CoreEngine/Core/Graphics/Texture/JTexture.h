#pragma once
#include "common_include.h"
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/FileIO/JSerialization.h"

/**
 * 텍스처 관련 리소스 집합
 * GetWorld->TextureManager로 관리된다.
 */
class JTexture final
{
public:
	explicit JTexture(JWTextView InName, bool bEnableEdit = false);
	explicit JTexture(JTextView InName, bool bEnableEdit = false);
	~JTexture();

public:
	static void SetShaderTexture2D(ID3D11DeviceContext* InDeviceContext, uint32_t InSlot = 0,
								   JTexture*            InTexture                        = nullptr);

public:
	[[nodiscard]] FORCEINLINE JWText                          GetTextureName() const { return mTextureName; }
	[[nodiscard]] FORCEINLINE uint32_t                        GetSlot() const { return mSlot; }
	[[nodiscard]] FORCEINLINE ID3D11ShaderResourceView*       GetSRV() const { return mShaderResourceView.Get(); }
	[[nodiscard]] FORCEINLINE D3D11_SHADER_RESOURCE_VIEW_DESC GetSrvDesc() const { return mSRVDesc; }
	[[nodiscard]] FORCEINLINE D3D11_TEXTURE2D_DESC            GetTextureDesc() const { return mTextureDesc; }
	[[nodiscard]] FORCEINLINE JArray<float>                   GetRGBAData() const { return mRGBAData; }

	/** 셰이더의 특정 슬롯에 이 텍스처를 할당하려면 이 메서드를 호출 (default slot = 0) */
	FORCEINLINE void SetShaderSlot(const uint32_t InSlotIndex) { mSlot = InSlotIndex; }

public:
	void SetEditable();

private:
	/** 경로String으로 텍스처를 생성 */
	void LoadFromFile();
	void LoadFromFileEx();
	void CacheRGBAData(const ComPtr<ID3D11Texture2D>& InTex2D);

private:
	uint32_t mSlot;
	JWText   mTextureName /** 실제 텍스처 경로 */;

	ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
	D3D11_SHADER_RESOURCE_VIEW_DESC  mSRVDesc;
	D3D11_TEXTURE2D_DESC             mTextureDesc; // 크기, 형식, 샘플링 등
public:
	JArray<float>                    mRGBAData;
};

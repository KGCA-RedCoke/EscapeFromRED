#include "common_pch.h"
#include "JTexture.h"
#include "Core/Graphics/GraphicDevice.h"
#include <directxtk/WICTextureLoader.h>

#include "Core/Utils/Utils.h"

JTexture::JTexture(JWTextView InName)
	: mID(StringHash(InName.data())),
	  mTextureName(InName),
	  mSRVDesc(),
	  mTextureDesc()
{
	mSlot = 0;
	LoadFromFile();
}

JTexture::JTexture(JTextView InName)
	: JTexture(String2WString(InName.data())) {}

void JTexture::PreRender(int32_t InSlot )
{
	if (mShaderResourceView.Get())
		G_Context.GetImmediateDeviceContext()->PSSetShaderResources(InSlot, 1, mShaderResourceView.GetAddressOf());
}


void JTexture::LoadFromFile()
{
	ComPtr<ID3D11Resource>  textureResource;
	ComPtr<ID3D11Texture2D> texture;


	CheckResult(CreateWICTextureFromFile(G_Context.GetDevice(),
										 mTextureName.c_str(),
										 textureResource.GetAddressOf(),
										 mShaderResourceView.GetAddressOf()));

	CheckResult(textureResource->QueryInterface(__uuidof(ID3D11Texture2D),
												reinterpret_cast<void**>(texture.GetAddressOf())));

	mShaderResourceView->GetDesc(&mSRVDesc);
	texture->GetDesc(&mTextureDesc);

	textureResource = nullptr;
	texture         = nullptr;
}

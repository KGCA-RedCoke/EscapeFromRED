#include "JTexture.h"
#include <directxtk/WICTextureLoader.h>

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"

#include "Core/Utils/Utils.h"

JTexture::JTexture(JWTextView InName, bool bEnableEdit)
	: mSlot(0),
	  mTextureName(InName),
	  mSRVDesc(),
	  mTextureDesc()
{

	if (bEnableEdit)
	{
		LoadFromFileEx();
	}
	else
	{
		LoadFromFile();
	}
}

JTexture::JTexture(JTextView InName, bool bEnableEdit)
	: JTexture(String2WString(InName.data()), bEnableEdit) {}

JTexture::~JTexture() {}

void JTexture::SetShaderTexture2D(ID3D11DeviceContext* InDeviceContext, uint32_t InSlot,
								  JTexture*            InTexture)
{
	ID3D11ShaderResourceView* srv = nullptr;
	if (InTexture)
	{
		srv = InTexture->mShaderResourceView.Get();
	}

	InDeviceContext->PSSetShaderResources(InSlot, 1, &srv);

}


void JTexture::SetEditable()
{
	mShaderResourceView = nullptr;
	mRGBAData.clear();

	LoadFromFileEx();
}

void JTexture::LoadFromFile()
{
	ComPtr<ID3D11Resource>  textureResource;
	ComPtr<ID3D11Texture2D> texture;


	if (FAILED(CreateWICTextureFromFileEx(
				   G_Device.GetDevice(),
				   mTextureName.c_str(),
				   0,
				   D3D11_USAGE_DEFAULT,
				   D3D11_BIND_SHADER_RESOURCE,
				   0,
				   0,
				   WIC_LOADER_FORCE_RGBA32,
				   textureResource.GetAddressOf(),
				   mShaderResourceView.GetAddressOf()
			   )))
	// 여기서는 CheckResult를 하지 않는다. 텍스처 파일이 없을 경우 디폴트 텍스처 적용
	// if (FAILED(CreateWICTextureFromFile(
	// 			   G_Device.GetDevice(),
	// 			   mTextureName.c_str(),
	// 			   textureResource.GetAddressOf(),
	// 			   mShaderResourceView.GetAddressOf())))
	{
		JText errorText = WString2String(std::format(L"Texture Load Failed: {}", mTextureName));
		// ShowErrorPopup(errorText);
		return;
	}

	CheckResult(textureResource->QueryInterface(__uuidof(ID3D11Texture2D),
												reinterpret_cast<void**>(texture.GetAddressOf())));

	mShaderResourceView->GetDesc(&mSRVDesc);
	texture->GetDesc(&mTextureDesc);

	textureResource = nullptr;
	texture         = nullptr;
}

void JTexture::LoadFromFileEx()
{
	ComPtr<ID3D11Resource>  textureResource;
	ComPtr<ID3D11Texture2D> texture;

	if (FAILED(CreateWICTextureFromFileEx(
				   G_Device.GetDevice(),
				   mTextureName.c_str(),
				   0,
				   D3D11_USAGE_STAGING,
				   0,
				   D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
				   0,
				   WIC_LOADER_FORCE_SRGB,
				   textureResource.GetAddressOf(),
				   nullptr
			   )))
	{
		JText errorText = WString2String(std::format(L"Texture Load Failed: {}", mTextureName));
		// ShowErrorPopup(errorText);
		return;
	}

	CheckResult(textureResource->QueryInterface(__uuidof(ID3D11Texture2D),
												reinterpret_cast<void**>(texture.GetAddressOf())));

	// mShaderResourceView->GetDesc(&mSRVDesc);
	texture->GetDesc(&mTextureDesc);

	CacheRGBAData(texture);

	textureResource = nullptr;
	texture         = nullptr;
}

void JTexture::CacheRGBAData(const ComPtr<ID3D11Texture2D>& InTex2D)
{
	auto context = G_Device.GetImmediateDeviceContext();
	assert(context);

	mRGBAData.resize(mTextureDesc.Width * mTextureDesc.Height);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(context->Map(InTex2D.Get(), D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ, 0, &mappedResource)))
	{
		unsigned char* data = static_cast<unsigned char*>(mappedResource.pData);

		for (int32_t row = 0; row < mTextureDesc.Height; ++row)
		{
			uint32_t rowStart = row * mappedResource.RowPitch;
			for (int32_t column = 0; column < mTextureDesc.Width; ++column)
			{
				uint32_t columnStart = column * 4;

				unsigned char r = data[rowStart + columnStart];
				unsigned char g = data[rowStart + columnStart + 1];
				unsigned char b = data[rowStart + columnStart + 2];

				mRGBAData[row * mTextureDesc.Width + column] = static_cast<float>(r + g + b) / 3.0f;
			};
		}
		context->Unmap(InTex2D.Get(), D3D11CalcSubresource(0, 0, 1));
	}
}

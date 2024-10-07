#include "JTexture.h"
#include <directxtk/WICTextureLoader.h>
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/MManagerInterface.h"

#include "Core/Utils/Utils.h"

JTexture::JTexture()
	: mSlot(0),
	  mSRVDesc(),
	  mTextureDesc()
{}

JTexture::JTexture(JWTextView InName)
	: mSlot(0),
	  mTextureName(InName),
	  mSRVDesc(),
	  mTextureDesc()
{

	LoadFromFile();
}

JTexture::JTexture(JTextView InName)
	: JTexture(String2WString(InName.data())) {}


uint32_t JTexture::GetHash() const
{
	return StringHash(ParseFile(mTextureName).c_str());
}

uint32_t JTexture::GetType() const
{
	return StringHash("JTexture");
}

bool JTexture::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// Slot
	Utils::Serialization::Serialize_Primitive(&mSlot, sizeof(mSlot), FileStream);

	// Texture Name
	Utils::Serialization::Serialize_Text(mTextureName, FileStream);

	return true;
}

bool JTexture::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// Slot
	Utils::Serialization::DeSerialize_Primitive(&mSlot, sizeof(mSlot), InFileStream);

	// Texture Name
	Utils::Serialization::DeSerialize_Text(mTextureName, InFileStream);

	LoadFromFile();

	return true;
}

void JTexture::PreRender(int32_t InSlot)
{
	if (mShaderResourceView.Get())
		IManager.RenderManager->GetImmediateDeviceContext()->PSSetShaderResources(InSlot, 1, mShaderResourceView.GetAddressOf());
}


void JTexture::LoadFromFile()
{
	ComPtr<ID3D11Resource>  textureResource;
	ComPtr<ID3D11Texture2D> texture;

	// 여기서는 CheckResult를 하지 않는다. 텍스처 파일이 없을 경우 디폴트 텍스처 적용
	if (FAILED(CreateWICTextureFromFile(Renderer.GetDevice(),
				   mTextureName.c_str(),
				   textureResource.GetAddressOf(),
				   mShaderResourceView.GetAddressOf())))
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

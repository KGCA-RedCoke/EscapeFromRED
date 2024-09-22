#include "JTexture.h"
#include "Core/Graphics/XD3DDevice.h"
#include <directxtk/WICTextureLoader.h>

#include "Core/Utils/Utils.h"

JTexture::JTexture()
	: mSlot(0),
	  mID(0),
	  mSRVDesc(),
	  mTextureDesc() {}

JTexture::JTexture(JWTextView InName)
	: mSlot(0),
	  mID(StringHash(InName.data())),
	  mTextureName(InName),
	  mSRVDesc(),
	  mTextureDesc()
{
	LoadFromFile();
}

JTexture::JTexture(JTextView InName)
	: JTexture(String2WString(InName.data())) {}


void JTexture::Serialize(std::ofstream& FileStream)
{
	/*// Header
	JAssetHeader header;
	header.DataSize      = sizeof(JAssetHeader) + sizeof(uint32_t);
	header.InstanceCount = 3;
	FileStream.write(reinterpret_cast<const char*>(&header), sizeof(JAssetHeader));*/

	// ID
	FileStream.write(reinterpret_cast<const char*>(&mID), sizeof(mID));

	// Slot
	FileStream.write(reinterpret_cast<const char*>(&mSlot), sizeof(mSlot));

	// Texture Name
	Utils::Serialization::Serialize_Text(mTextureName, FileStream);
}

void JTexture::DeSerialize(std::ifstream& InFileStream)
{
	/*// Header
	JAssetHeader header;
	InFileStream.read(reinterpret_cast<char*>(&header), sizeof(JAssetHeader));

	if (StringHash(header.Signature) != JAssetHash)
	{
		LOG_CORE_ERROR("Header signature is not valid");
		InFileStream.close();
		return;
	}*/

	// ID
	InFileStream.read(reinterpret_cast<char*>(&mID), sizeof(mID));

	// Slot
	InFileStream.read(reinterpret_cast<char*>(&mSlot), sizeof(mSlot));

	// Texture Name
	Utils::Serialization::DeSerialize_Text(mTextureName, InFileStream);

	LoadFromFile();
}

void JTexture::PreRender(int32_t InSlot)
{
	if (mShaderResourceView.Get())
		DeviceRSC.GetImmediateDeviceContext()->PSSetShaderResources(InSlot, 1, mShaderResourceView.GetAddressOf());
}


void JTexture::LoadFromFile()
{
	ComPtr<ID3D11Resource>  textureResource;
	ComPtr<ID3D11Texture2D> texture;

	// 여기서는 CheckResult를 하지 않는다. 텍스처 파일이 없을 경우 디폴트 텍스처 적용
	if (FAILED(CreateWICTextureFromFile(DeviceRSC.GetDevice(),
				   mTextureName.c_str(),
				   textureResource.GetAddressOf(),
				   mShaderResourceView.GetAddressOf())))
	{
		return;
	}

	CheckResult(textureResource->QueryInterface(__uuidof(ID3D11Texture2D),
												reinterpret_cast<void**>(texture.GetAddressOf())));

	mShaderResourceView->GetDesc(&mSRVDesc);
	texture->GetDesc(&mTextureDesc);

	textureResource = nullptr;
	texture         = nullptr;
}

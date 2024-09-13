#include "common_pch.h"
#include "JTexture.h"
#include "Core/Graphics/GraphicDevice.h"
#include <directxtk/WICTextureLoader.h>

#include "Core/Utils/Utils.h"

JTexture::JTexture() {}

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
	const JText  rawString = WString2String(mTextureName);
	const size_t nameSize  = rawString.length();
	FileStream.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
	FileStream.write(reinterpret_cast<const char*>(rawString.data()), nameSize);
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
	size_t nameSize;
	InFileStream.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));

	JText rawString(nameSize,'0');
	InFileStream.read(reinterpret_cast<char*>(rawString.data()), nameSize);
	mTextureName = String2WString(rawString);

	LoadFromFile();
}

void JTexture::PreRender(int32_t InSlot)
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

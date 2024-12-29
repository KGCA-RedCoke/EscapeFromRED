#include "MAnimManager.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Utils/Graphics/DXUtils.h"

void MAnimatorManager::PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
									  void*        Entity)
{
	auto* animator = static_cast<JAnimator*>(Entity);

	if (std::filesystem::is_regular_file(OriginalNameOrPath))
	{
		Utils::Serialization::DeSerialize(OriginalNameOrPath.data(), animator);
	}
	else
	{
		Utils::Serialization::Serialize(OriginalNameOrPath.data(), animator);
	}
}

MAnimatorManager::MAnimatorManager() {}

MAnimatorManager::~MAnimatorManager() {}

void MAnimManager::PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
								  void*        Entity)
{
	auto* animClip = static_cast<JAnimationClip*>(Entity);

	if (std::filesystem::is_regular_file(OriginalNameOrPath))
	{
		Utils::Serialization::DeSerialize(OriginalNameOrPath.data(), animClip);

		auto* skeletal = animClip->GetSkeletalMesh();
		if (!skeletal)
			return;

		mAnimOffset[ParsedName] = animClip->GenerateAnimationTexture(mAnimTextures_Skeletal[skeletal]);

		auto it = mAnimTextureBuffer_SRV.find(skeletal);
		if (it != mAnimTextureBuffer_SRV.end())
		{
			mAnimTextureBuffer[skeletal]     = nullptr;
			mAnimTextureBuffer_SRV[skeletal] = nullptr;

		}
		Utils::DX::CreateBuffer(
								G_Device.GetDevice(),
								D3D11_BIND_SHADER_RESOURCE,
								reinterpret_cast<void**>(mAnimTextures_Skeletal[skeletal].data()),
								sizeof(FVector4),
								mAnimTextures_Skeletal[skeletal].size(),
								mAnimTextureBuffer[skeletal].GetAddressOf(),
								D3D11_USAGE_DEFAULT,
								0,
								D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
								sizeof(FVector4));

		D3D11_SHADER_RESOURCE_VIEW_DESC boneSRVDesc;
		boneSRVDesc.Format               = DXGI_FORMAT_UNKNOWN;
		boneSRVDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
		boneSRVDesc.Buffer.ElementOffset = 0;
		boneSRVDesc.Buffer.ElementWidth  = mAnimTextures_Skeletal[skeletal].size();

		CheckResult(G_Device.GetDevice()->CreateShaderResourceView(mAnimTextureBuffer[skeletal].Get(),
																   &boneSRVDesc,
																   mAnimTextureBuffer_SRV[skeletal].GetAddressOf()));
	}
}

void MAnimManager::BindAnimationTexture(ID3D11DeviceContext* InContext, JSkeletalMesh* InSkeletalMesh)
{
	InContext->VSSetShaderResources(10,
									1,
									mAnimTextureBuffer_SRV[InSkeletalMesh].GetAddressOf());
}

MAnimManager::MAnimManager() {}
MAnimManager::~MAnimManager() {}

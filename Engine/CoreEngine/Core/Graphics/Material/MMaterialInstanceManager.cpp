#include "MMaterialInstanceManager.h"

#include "MMaterialManager.h"
#include "Core/Graphics/XD3DDevice.h"

void MMaterialInstanceManager::PostInitialize(const JText&   OriginalNameOrPath, const JText& ParsedName,
											  const uint32_t NameHash, void*                  Entity)
{
	JMaterialInstance* materialInstance = static_cast<JMaterialInstance*>(Entity);
	assert(materialInstance);

	if (Utils::Serialization::IsJAssetFileAndExist(OriginalNameOrPath.c_str()))
	{
		Utils::Serialization::DeSerialize(OriginalNameOrPath.c_str(), materialInstance);
	}
	else
	{
		mManagedList[NameHash]->SetParentMaterial(MMaterialManager::Get().Load(NAME_MAT_BASIC));
	}

	//
	// JMaterial* parentMaterial = materialInstance->GetParentMaterial();
	// assert(parentMaterial);
	//
	// auto it = mBufferList.find(parentMaterial);
	// if (it == mBufferList.end())
	// {
	// 	uint32_t materialSize = materialInstance->GetMaterialSize();
	// 	if (materialSize == 0)
	// 	{
	// 		LOG_CORE_ERROR("Material Size is 0");
	// 		return;
	// 	}
	//
	// 	Utils::DX::CreateBuffer(G_Device.GetDevice(),
	// 							D3D11_BIND_SHADER_RESOURCE,
	// 							nullptr,
	// 							materialSize,
	// 							255,
	// 							mBufferList[parentMaterial].GetAddressOf(),
	// 							D3D11_USAGE_DYNAMIC,
	// 							D3D11_CPU_ACCESS_WRITE,
	// 							D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
	// 							materialSize);
	//
	// 	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	// 	{
	// 		srvDesc.Format               = DXGI_FORMAT_UNKNOWN;
	// 		srvDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
	// 		srvDesc.Buffer.ElementOffset = 0;
	// 		srvDesc.Buffer.ElementWidth  = materialInstance->GetMaterialSize();
	// 	}
	//
	// 	CheckResult( G_Device.GetDevice()->CreateShaderResourceView(mBufferList[parentMaterial].Get(),
	// 												   &srvDesc,
	// 												   mSRVList[parentMaterial].GetAddressOf()));
	// }
}

void MMaterialInstanceManager::UpdateMaterialInstance(const JMaterialInstance* InMaterialInstance)
{
	JMaterial* parentMaterial = InMaterialInstance->GetParentMaterial();
	assert(parentMaterial);

	auto it = mBufferList.find(parentMaterial);
	if (it != mBufferList.end())
	{
		auto* data = InMaterialInstance->GetMaterialData();
		if (!data)
		{
			return;
		}

		Utils::DX::UpdateDynamicBuffer(G_Device.GetImmediateDeviceContext(),
									   mBufferList[parentMaterial].Get(),
									   data,
									   InMaterialInstance->GetMaterialSize());

		G_Device.GetImmediateDeviceContext()->PSSetShaderResources(
																   SLOT_MATERIAL,
																   1,
																   mSRVList[parentMaterial].GetAddressOf());
	}

}

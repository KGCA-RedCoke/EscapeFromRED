#include "JMaterialInstance.h"

#include "Core/Graphics/Material/JMaterial_Basic.h"
#include "Core/Graphics/Material/MMaterialManager.h"
#include "Core/Graphics/Shader/InputLayouts.h"
#include "Core/Graphics/Shader/JShader_Basic.h"
#include "Core/Interface/MManagerInterface.h"

JMaterialInstance::JMaterialInstance(JTextView InName)
	: mFileName(InName)
{
	if (std::filesystem::is_regular_file(mFileName))
	{
		Utils::Serialization::DeSerialize(mFileName.c_str(), this);
	}
	else
	{
		SetParentMaterial(MMaterialManager::Get().CreateOrLoad<JMaterial_Basic>(NAME_MAT_BASIC));
	}
}

JMaterialInstance::JMaterialInstance(JWTextView InName)
	: JMaterialInstance(WString2String(InName.data())) {}

Ptr<IManagedInterface> JMaterialInstance::Clone() const
{
	return nullptr;
}

uint32_t JMaterialInstance::GetHash() const
{
	return StringHash(mFileName.c_str());
}

uint32_t JMaterialInstance::GetType() const
{
	return StringHash("JMaterialInstance");
}

bool JMaterialInstance::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// Parent Material Path
	Utils::Serialization::Serialize_Text(mParentMaterial->mMaterialPath, FileStream);

	// Instance Params
	int32_t paramCount = mInstanceParams.size();
	Utils::Serialization::Serialize_Primitive(&paramCount, sizeof(paramCount), FileStream);
	for (int32_t i = 0; i < paramCount; ++i)
	{
		mInstanceParams[i].Serialize_Implement(FileStream);
	}

	return true;
}

bool JMaterialInstance::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// Parent Material Path
	JText parentMaterialPath;
	Utils::Serialization::DeSerialize_Text(parentMaterialPath, InFileStream);
	mParentMaterial = MMaterialManager::Get().CreateOrLoad(parentMaterialPath);

	// Get Instance Params
	int32_t paramCount;
	Utils::Serialization::DeSerialize_Primitive(&paramCount, sizeof(paramCount), InFileStream);
	mInstanceParams.reserve(paramCount);

	for (int32_t i = 0; i < paramCount; ++i)
	{
		FMaterialParam param;
		param.DeSerialize_Implement(InFileStream);
		mInstanceParams.push_back(param);
	}

	return true;
}

void JMaterialInstance::BindMaterial(ID3D11DeviceContext* InDeviceContext)
{
	mParentMaterial->mShader->BindShaderPipeline(InDeviceContext);

	for (int32_t i = 0; i < mInstanceParams.size(); ++i)
	{
		mInstanceParams[i].BindMaterialParam();
	}
}

void JMaterialInstance::BindMaterialBuffer(ID3D11DeviceContext* InDeviceContext, const uint32_t InSlot)
{
	InDeviceContext->PSSetConstantBuffers(InSlot, 1, mMaterialBuffer.GetAddressOf());
}

void JMaterialInstance::UpdateWorldMatrix(ID3D11DeviceContext* InDeviceContext, const FMatrix& InWorldMatrix) const
{
	mParentMaterial->mShader->SetObject(InDeviceContext, InWorldMatrix);
}

void JMaterialInstance::UpdateCamera(ID3D11DeviceContext* InDeviceContext, const Ptr<JCamera>& InCameraObj) const
{
	mParentMaterial->mShader->SetCameraData(InDeviceContext, InCameraObj);
}

void JMaterialInstance::UpdateLightColor(ID3D11DeviceContext* InDeviceContext, const FVector4& InLightColor) const
{
	mParentMaterial->mShader->SetDirectionalLightColor(InDeviceContext, InLightColor);
}

void JMaterialInstance::UpdateLightLoc(ID3D11DeviceContext* InDeviceContext, const FVector4& InLightLoc) const
{
	mParentMaterial->mShader->SetDirectionalLightPos(InDeviceContext, InLightLoc);
}

void JMaterialInstance::GenerateMaterialBuffer()
{
	Utils::DX::CreateBuffer(IManager.RenderManager->GetDevice(),
							D3D11_BIND_CONSTANT_BUFFER,
							nullptr,
							mParentMaterial->mMaterialBufferSize,
							1,
							mMaterialBuffer.GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE);
}

void JMaterialInstance::GetInstanceParams()
{
	for (int32_t i = 0; i < mParentMaterial->mMaterialParams.size(); ++i)
	{
		if (mParentMaterial->mMaterialParams[i].bInstanceParam)
		{
			mInstanceParams.push_back(mParentMaterial->mMaterialParams[i]);
		}
	}
}

void JMaterialInstance::SetParentMaterial(const Ptr<JMaterial>& InParentMaterial)
{
	mParentMaterial = nullptr;
	mMaterialBuffer = nullptr;
	mInstanceParams.clear();

	mParentMaterial = InParentMaterial;

	GenerateMaterialBuffer();
	GetInstanceParams();
}

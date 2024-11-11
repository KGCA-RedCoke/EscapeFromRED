#include "JMaterialInstance.h"

#include "Core/Graphics/Material/JMaterial_Basic.h"
#include "Core/Graphics/Material/MMaterialManager.h"
#include "Core/Graphics/Shader/InputLayouts.h"
#include "Core/Graphics/Shader/JShader_Basic.h"
#include "Core/Interface/MManagerInterface.h"

JMaterialInstance::JMaterialInstance(JTextView InName)
	: mFileName(InName)
{
	if (std::filesystem::exists(mFileName) && std::filesystem::is_regular_file(mFileName))
	{
		Utils::Serialization::DeSerialize(mFileName.c_str(), this);
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
	if (!mParentMaterial)
	{
		JText empty = "NONE";
		Utils::Serialization::Serialize_Text(empty, FileStream);
	}
	else
	{
		Utils::Serialization::Serialize_Text(mParentMaterial->mMaterialPath, FileStream);
	}

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
	if (!parentMaterialPath.empty())
	{
		mParentMaterial = MMaterialManager::Get().CreateOrLoad(parentMaterialPath);
		mShader         = mParentMaterial->GetShader();
	}

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
	mParentMaterial->BindMaterialPipeline(InDeviceContext, mInstanceParams	);
	// const auto shaderRef = mShader.lock();
	// if (!shaderRef)
	// 	return;
	// shaderRef->BindShaderPipeline(InDeviceContext);
	//
	// if (mInstanceParams.empty())
	// {
	// 	shaderRef->UpdateConstantData(IManager.RenderManager->GetImmediateDeviceContext(),
	// 								  CBuffer::NAME_CONSTANT_BUFFER_MATERIAL,
	// 								  nullptr);
	// }
	//
	// for (int32_t i = 0; i < mInstanceParams.size(); ++i)
	// {
	// 	FMaterialParam& param = mInstanceParams[i];
	//
	// 	shaderRef->UpdateConstantData(IManager.RenderManager->GetImmediateDeviceContext(),
	// 								  CBuffer::NAME_CONSTANT_BUFFER_MATERIAL,
	// 								  param.Name,
	// 								  &param.Float4Value);
	//
	// 	param.BindMaterialParam(InDeviceContext, i);
	// }
}

void JMaterialInstance::UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName,
										   const void*          InData, const uint32_t        InOffset) const
{
	if (mShader.expired())
		return;

	mShader.lock()->UpdateConstantData(InDeviceContext, InBufferName, InData, InOffset);
}

void JMaterialInstance::UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName,
										   const JText&         InDataName, const void*       InData) const
{
	if (mShader.expired())
		return;

	mShader.lock()->UpdateConstantData(InDeviceContext, InBufferName, InDataName, InData);
}

void JMaterialInstance::UpdateWorldMatrix(ID3D11DeviceContext* InDeviceContext, const FMatrix& InWorldMatrix) const
{
	if (mShader.expired())
		return;

	mShader.lock()->UpdateConstantData(InDeviceContext,
									   CBuffer::NAME_CONSTANT_BUFFER_SPACE,
									   CBuffer::NAME_CONSTANT_VARIABLE_SPACE_WORLD,
									   &InWorldMatrix);
}

void JMaterialInstance::UpdateCamera(ID3D11DeviceContext* InDeviceContext, const Ptr<JCamera>& InCameraObj) const
{
	if (mShader.expired())
		return;
	// mParentMaterial->mShader->
	FVector4 camPos = {InCameraObj->GetEyePositionFVector(), 1.f};
	mShader.lock()->UpdateConstantData(InDeviceContext,
									   CBuffer::NAME_CONSTANT_BUFFER_CAMERA,
									   CBuffer::NAME_CONSTANT_VARIABLE_CAMERA_POS,
									   &camPos);
}

void JMaterialInstance::UpdateLightColor(ID3D11DeviceContext* InDeviceContext, const FVector4& InLightColor) const
{
	if (mShader.expired())
		return;
	mShader.lock()->UpdateConstantData(InDeviceContext,
									   CBuffer::NAME_CONSTANT_BUFFER_LIGHT,
									   CBuffer::NAME_CONSTANT_VARIABLE_LIGHT_COLOR,
									   &InLightColor);
}

void JMaterialInstance::UpdateLightLoc(ID3D11DeviceContext* InDeviceContext, const FVector4& InLightLoc) const
{
	if (!mParentMaterial)
		return;
	mParentMaterial->mShader->UpdateConstantData(InDeviceContext,
												 CBuffer::NAME_CONSTANT_BUFFER_LIGHT,
												 CBuffer::NAME_CONSTANT_VARIABLE_LIGHT_POS,
												 &InLightLoc);
}

FMaterialParam* JMaterialInstance::GetInstanceParam(const JText& InParamName)
{
	if (mInstanceParams.empty())
	{
		return nullptr;
	}

	const uint32_t hash = StringHash(InParamName.c_str());

	for (auto& param : mInstanceParams)
	{
		if (param.Key == hash)
		{
			return &param;
		}
	}
	return nullptr;
}

void JMaterialInstance::AddInstanceParam(const FMaterialParam& InParamValue)
{
	if (GetInstanceParam(InParamValue.Name))
	{
		LOG_CORE_ERROR("Material Instance Param already exists.");
		return;
	}

	mInstanceParams.push_back(InParamValue);
}

void JMaterialInstance::EditInstanceParam(const JText& InParamName, const FMaterialParam& InParamValue)
{
	if (mInstanceParams.empty())
	{
		return;
	}

	const uint32_t hash = StringHash(InParamName.c_str());

	for (auto& param : mInstanceParams)
	{
		if (param.Key == hash)
		{
			param = InParamValue;
			return;
		}
	}
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
	assert(InParentMaterial);

	mParentMaterial = nullptr;
	mInstanceParams.clear();

	mParentMaterial = InParentMaterial;
	mShader         = mParentMaterial->GetShader();

	GetInstanceParams();
}

#include "JMaterialInstance.h"

#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Graphics/Material/JMaterial_Basic.h"
#include "Core/Graphics/Material/MMaterialInstanceManager.h"
#include "Core/Graphics/Material/MMaterialManager.h"
#include "Core/Graphics/Shader/InputLayouts.h"
#include "Core/Graphics/Shader/JShader_Basic.h"
#include "Core/Interface/JWorld.h"

JMaterialInstance::JMaterialInstance(JTextView InName)
	: mFileName(InName)
{}

JMaterialInstance::JMaterialInstance(JWTextView InName)
	: JMaterialInstance(WString2String(InName.data())) {}

JMaterialInstance::JMaterialInstance(const JMaterialInstance& Other)
	: mFileName(std::format("{0}_Copy.jasset", Other.mFileName)),
	  mParentMaterial(Other.mParentMaterial),
	  mShader(Other.mShader)
{
	mInstanceParams.clear();
	mInstanceParams.reserve(Other.mInstanceParams.size());

	for (const auto& param : Other.mInstanceParams)
	{
		mInstanceParams.push_back(param);
	}
}

UPtr<IManagedInterface> JMaterialInstance::Clone() const
{
	return MakeUPtr<JMaterialInstance>(*this);
}

void JMaterialInstance::SetAsDefaultMaterial()
{
	mParentMaterial = MMaterialManager::Get().CreateOrLoad(NAME_MAT_BASIC);
	mShader         = mParentMaterial->GetShader();

	GetInstanceParams();

	JMaterialInstance* defaultInstance = MMaterialInstanceManager::Get().CreateOrLoad(NAME_MAT_INS_DEFAULT);

	for (int32_t i = 0; i < defaultInstance->mInstanceParams.size(); ++i)
	{
		mInstanceParams[i] = defaultInstance->mInstanceParams[i];
	}
}

uint32_t JMaterialInstance::GetHash() const
{
	return StringHash(mFileName.c_str());
}

uint32_t JMaterialInstance::GetType() const
{
	return HASH_ASSET_TYPE_MATERIAL_INSTANCE;
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
		mParentMaterial = MMaterialManager::Get().CreateOrLoad(NAME_MAT_BASIC);
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
	mInstanceParams.clear();
	mInstanceParams.reserve(paramCount);

	for (int32_t i = 0; i < paramCount; ++i)
	{
		FMaterialParam param;
		param.DeSerialize_Implement(InFileStream);
		mInstanceParams.push_back(param);
	}

	return true;
}

void JMaterialInstance::BindMaterial(ID3D11DeviceContext* InDeviceContext) const
{
	mParentMaterial->BindMaterialPipeline(InDeviceContext, mInstanceParams);
}

void JMaterialInstance::UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName,
										   const void*          InData, const uint32_t        InOffset) const
{
	if (!mShader)
		return;

	mShader->UpdateConstantData(InDeviceContext, InBufferName, InData, InOffset);
}

void JMaterialInstance::UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName,
										   const JText&         InDataName, const void*       InData) const
{
	if (!mShader)
		return;

	mShader->UpdateConstantData(InDeviceContext, InBufferName, InDataName, InData);
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
			OnMaterialInstanceParamChanged.Execute();
			return;
		}
	}
}

void JMaterialInstance::GetInstanceParams()
{
	mInstanceParams.clear();
	for (int32_t i = 0; i < mParentMaterial->mMaterialParams.size(); ++i)
	{
		if (mParentMaterial->mMaterialParams[i].bInstanceParam)
		{
			mInstanceParams.push_back(mParentMaterial->mMaterialParams[i]);
		}
	}
}

void JMaterialInstance::SetParentMaterial(JMaterial* InParentMaterial)
{
	assert(InParentMaterial);

	mParentMaterial = nullptr;
	mInstanceParams.clear();

	mParentMaterial = InParentMaterial;
	mShader         = mParentMaterial->GetShader();

	GetInstanceParams();
}

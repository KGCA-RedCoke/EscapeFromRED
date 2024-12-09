#include "JMaterialInstance.h"

#include <ranges>

#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Graphics/Material/JMaterial_Basic.h"
#include "Core/Graphics/Material/MMaterialManager.h"
#include "Core/Graphics/Shader/InputLayouts.h"
#include "Core/Graphics/Shader/JShader_Basic.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/JWorld.h"

JMaterialInstance::JMaterialInstance(JTextView InName)
	: mFileName(InName)
{
	mPath = InName;
}

JMaterialInstance::JMaterialInstance(JWTextView InName)
	: JMaterialInstance(WString2String(InName.data())) {}

JMaterialInstance::JMaterialInstance(const JMaterialInstance& Other)
	: mFileName(std::format("{0}_Copy.jasset", Other.mFileName)),
	  mParentMaterial(Other.mParentMaterial),
	  mShader(Other.mShader)
{}

UPtr<IManagedInterface> JMaterialInstance::Clone() const
{
	return MakeUPtr<JMaterialInstance>(*this);
}

void JMaterialInstance::SetAsDefaultMaterial()
{
	// mParentMaterial = MMaterialManager::Get().Load(NAME_MAT_BASIC);
	// mShader         = mParentMaterial->GetShader();
	//
	// GetInstanceParams();
	//
	// JMaterialInstance* defaultInstance = MMaterialInstanceManager::Get().Load(NAME_MAT_INS_DEFAULT);
	//
	// for (int32_t i = 0; i < defaultInstance->mInstanceParams.size(); ++i)
	// {
	// 	mInstanceParams[i] = defaultInstance->mInstanceParams[i];
	// }
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
		mParentMaterial = MMaterialManager::Get().Load(NAME_MAT_BASIC);
	}
	else
	{
		Utils::Serialization::Serialize_Text(mParentMaterial->mMaterialPath, FileStream);
	}

	// Instance Params
	int32_t paramCount   = mMaterialParamToRawDataIndex.size();
	int32_t rawDataCount = mMaterialRawData.size();
	int32_t textureCount = mTextureMap.size();

	Utils::Serialization::Serialize_Primitive(&paramCount, sizeof(paramCount), FileStream);
	Utils::Serialization::Serialize_Primitive(&rawDataCount, sizeof(rawDataCount), FileStream);
	Utils::Serialization::Serialize_Primitive(&textureCount, sizeof(textureCount), FileStream);

	for (auto hash : mMaterialParamToRawDataIndex)
	{
		uint32_t key = hash.first;
		Utils::Serialization::Serialize_Primitive(&key, sizeof(hash.first), FileStream);
		Utils::Serialization::Serialize_Primitive(&hash.second, sizeof(hash.second), FileStream);
	}

	Utils::Serialization::Serialize_Primitive(mMaterialRawData.data(),
											  rawDataCount * sizeof(float),
											  FileStream);

	for (auto& texture : mTextureMap | std::views::values)
	{
		JText textureName = texture ? WString2String(texture->GetTextureName()) : "None";
		Utils::Serialization::Serialize_Text(textureName, FileStream);
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
		mParentMaterial = MMaterialManager::Get().Load(parentMaterialPath);
		mShader         = mParentMaterial->GetShader();
	}
	
	// Instance Params
	int32_t paramCount;
	int32_t rawDataCount;
	int32_t textureCount;
	Utils::Serialization::DeSerialize_Primitive(&paramCount, sizeof(paramCount), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&rawDataCount, sizeof(rawDataCount), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&textureCount, sizeof(textureCount), InFileStream);

	if (paramCount < 0 || rawDataCount < 0 || textureCount < 0)
	{
		return true;
	}
	
	for (int32_t i = 0; i < paramCount; ++i)
	{
		uint32_t key;
		uint32_t index;
		Utils::Serialization::DeSerialize_Primitive(&key, sizeof(key), InFileStream);
		Utils::Serialization::DeSerialize_Primitive(&index, sizeof(index), InFileStream);
	
		mMaterialParamToRawDataIndex[key] = index;
	}
	
	mMaterialRawData.resize(rawDataCount);
	Utils::Serialization::DeSerialize_Primitive(mMaterialRawData.data(),
												rawDataCount * sizeof(float),
												InFileStream);
	
	for (int32_t i = 0; i < textureCount; ++i)
	{
		JText textureName;
		Utils::Serialization::DeSerialize_Text(textureName, InFileStream);
		mTextureMap[i] = textureName == "None" ? nullptr : GetWorld.TextureManager->Load(textureName);
	}
	
	
	return true;
}

void JMaterialInstance::BindMaterial(ID3D11DeviceContext* InDeviceContext) const
{
	mParentMaterial->BindShader(InDeviceContext);

	for (auto& textureMap : mTextureMap)
	{
		JTexture::SetShaderTexture2D(InDeviceContext,
									 textureMap.first,
									 textureMap.second == nullptr
										 ? GetWorld.TextureManager->WhiteTexture
										 : textureMap.second);
	}
}

void* JMaterialInstance::GetInstanceParam(const JText& InParamName, bool bTextureParam)
{
	if (mMaterialRawData.empty())
	{
		return nullptr;
	}

	const uint32_t hash  = StringHash(InParamName.c_str());
	auto           it    = mMaterialParamToRawDataIndex.find(hash);
	const uint32_t index = it->second;

	if (it != mMaterialParamToRawDataIndex.end())
	{
		if (bTextureParam)
		{
			return mTextureMap[index];
		}

		return &mMaterialRawData[index];
	}

	return nullptr;
}

void* JMaterialInstance::GetInstanceParam(uint32_t InParamKey, bool bTextureParam)
{
	if (mMaterialRawData.empty())
	{
		return nullptr;
	}

	auto it = mMaterialParamToRawDataIndex.find(InParamKey);

	if (it != mMaterialParamToRawDataIndex.end())
	{
		const uint32_t index = it->second;
		if (bTextureParam)
		{
			return mTextureMap[index];
		}

		return &mMaterialRawData[index];
	}
	return nullptr;
}

void JMaterialInstance::EditInstanceParam(const JText& InParamName, const FMaterialParam& InParamValue)
{
	if (mMaterialRawData.empty())
	{
		return;
	}

	const uint32_t hash = StringHash(InParamName.c_str());

	for (auto& param : mParentMaterial->mMaterialParams)
	{
		if (param.Key == hash)
		{
			const uint32_t index = mMaterialParamToRawDataIndex[hash];
			switch (param.ParamValue)
			{
			case EMaterialParamValue::Boolean:
				mMaterialRawData[index] = InParamValue.BooleanValue;
				break;
			case EMaterialParamValue::Integer:
				mMaterialRawData[index] = InParamValue.IntegerValue;
				break;
			case EMaterialParamValue::Float:
				mMaterialRawData[index] = InParamValue.FloatValue;
				break;
			case EMaterialParamValue::Float2:
				mMaterialRawData[index] = InParamValue.Float2Value.x;
				mMaterialRawData[index + 1] = InParamValue.Float2Value.y;
				break;
			case EMaterialParamValue::Float3:
				mMaterialRawData[index] = InParamValue.Float3Value.x;
				mMaterialRawData[index + 1] = InParamValue.Float3Value.y;
				mMaterialRawData[index + 2] = InParamValue.Float3Value.z;
				break;
			case EMaterialParamValue::Float4:
				mMaterialRawData[index] = InParamValue.Float4Value.x;
				mMaterialRawData[index + 1] = InParamValue.Float4Value.y;
				mMaterialRawData[index + 2] = InParamValue.Float4Value.z;
				mMaterialRawData[index + 3] = InParamValue.Float4Value.w;
				break;
			case EMaterialParamValue::String:
				break;
			case EMaterialParamValue::Texture2D:
			case EMaterialParamValue::TextureCube:
			case EMaterialParamValue::TextureVolume:
				mTextureMap[index] = InParamValue.TextureValue;
				break;
			case EMaterialParamValue::Max:
				break;
			}
			OnMaterialInstanceParamChanged.Execute();
			return;
		}
	}


}

void JMaterialInstance::GetInstanceParams()
{
	mMaterialRawData.clear();
	mMaterialParamToRawDataIndex.clear();
	mTextureMap.clear();

	int32_t texSlot = 0;

	for (FMaterialParam& param : mParentMaterial->mMaterialParams)
	{
		switch (param.ParamValue)
		{
		case EMaterialParamValue::Boolean:
			mMaterialParamToRawDataIndex[param.Key] = mMaterialRawData.size();
			mMaterialRawData.push_back(param.BooleanValue);
			break;
		case EMaterialParamValue::Integer:
			mMaterialParamToRawDataIndex[param.Key] = mMaterialRawData.size();
			mMaterialRawData.push_back(param.IntegerValue);
			break;
		case EMaterialParamValue::Float:
			mMaterialParamToRawDataIndex[param.Key] = mMaterialRawData.size();
			mMaterialRawData.push_back(param.FloatValue);
			break;
		case EMaterialParamValue::Float2:
			mMaterialParamToRawDataIndex[param.Key] = mMaterialRawData.size();
			mMaterialRawData.push_back(param.Float2Value.x);
			mMaterialRawData.push_back(param.Float2Value.y);
			break;
		case EMaterialParamValue::Float3:
			mMaterialParamToRawDataIndex[param.Key] = mMaterialRawData.size();
			mMaterialRawData.push_back(param.Float3Value.x);
			mMaterialRawData.push_back(param.Float3Value.y);
			mMaterialRawData.push_back(param.Float3Value.z);
			break;
		case EMaterialParamValue::Float4:
			mMaterialParamToRawDataIndex[param.Key] = mMaterialRawData.size();
			mMaterialRawData.push_back(param.Float4Value.x);
			mMaterialRawData.push_back(param.Float4Value.y);
			mMaterialRawData.push_back(param.Float4Value.z);
			mMaterialRawData.push_back(param.Float4Value.w);
			break;
		case EMaterialParamValue::Texture2D:
			mMaterialParamToRawDataIndex[param.Key] = texSlot;
			mTextureMap[texSlot++] = param.TextureValue;
			break;
		default:
			break;
		}
	}
}

void JMaterialInstance::SetParentMaterial(JMaterial* InParentMaterial)
{
	assert(InParentMaterial);

	mParentMaterial = InParentMaterial;
	mShader         = mParentMaterial->GetShader();

	GetInstanceParams();
}

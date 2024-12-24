#include "JMaterial.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Shader/InputLayouts.h"
#include "Core/Graphics/Shader/JShader.h"
#include "Core/Graphics/Shader/JShader_Basic.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/Utils.h"


FMaterialParam::FMaterialParam(const JText& InName)
	: Name(InName)
{
	Key = StringHash(Name.c_str());
}

FMaterialParam::FMaterialParam(const JText& InName, EMaterialParamValue InParamValue, const void* InValue,
							   bool         bInInstanceParam)
	: FMaterialParam(InName)
{
	ParamValue     = InParamValue;
	bInstanceParam = bInInstanceParam;

	if (InParamValue == EMaterialParamValue::String)
	{
		StringValue = static_cast<const char*>(InValue);
	}
	else
	{
		switch (ParamValue)
		{
		case EMaterialParamValue::Boolean:
			BooleanValue = *static_cast<const bool*>(InValue);
			break;
		case EMaterialParamValue::Integer:
			IntegerValue = *static_cast<const int32_t*>(InValue);
			break;
		case EMaterialParamValue::Float:
			FloatValue = *static_cast<const float*>(InValue);
			break;
		case EMaterialParamValue::Float2:
			Float2Value = *static_cast<const FVector2*>(InValue);
			break;
		case EMaterialParamValue::Float3:
			Float3Value = *static_cast<const FVector*>(InValue);
			break;
		case EMaterialParamValue::Float4:
			Float4Value = *static_cast<const FVector4*>(InValue);
			break;
		default:
			break;
		}
	}
}

uint32_t FMaterialParam::GetType() const
{
	return StringHash("FMaterialParam");
}

bool FMaterialParam::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// Name
	Utils::Serialization::Serialize_Text(Name, FileStream);

	// bInstanceParam
	Utils::Serialization::Serialize_Primitive(&bInstanceParam, sizeof(bInstanceParam), FileStream);

	// ParamValue
	Utils::Serialization::Serialize_Primitive(&ParamValue, sizeof(ParamValue), FileStream);

	// Value
	Utils::Serialization::Serialize_Primitive(&Float4Value, sizeof(Float4Value), FileStream);
	Utils::Serialization::Serialize_Text(StringValue, FileStream);

	return true;
}

bool FMaterialParam::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// Name
	Utils::Serialization::DeSerialize_Text(Name, InFileStream);

	// bInstanceParam
	Utils::Serialization::DeSerialize_Primitive(&bInstanceParam, sizeof(bInstanceParam), InFileStream);

	// ParamValue
	Utils::Serialization::DeSerialize_Primitive(&ParamValue, sizeof(ParamValue), InFileStream);

	// Value
	Utils::Serialization::DeSerialize_Primitive(&Float4Value, sizeof(Float4Value), InFileStream);
	Utils::Serialization::DeSerialize_Text(StringValue, InFileStream);

	if (ParamValue == EMaterialParamValue::Texture2D && !StringValue.empty())
	{
		TextureValue = GetWorld.TextureManager->Load(StringValue.c_str());
	}

	Key = StringHash(Name.c_str());


	return true;
}

void FMaterialParam::BindMaterialParam(ID3D11DeviceContext* InDeviceContext, const uint32_t Index) const
{
	if (ParamValue == EMaterialParamValue::Texture2D && TextureValue)
	{
		JTexture::SetShaderTexture2D(InDeviceContext, Index, TextureValue);
	}
}

JMaterial::JMaterial(JTextView InMaterialName)
{
	mMaterialPath = {InMaterialName.begin(), InMaterialName.end()};
	mPath         = mMaterialPath;
	mMaterialName = ParseFile(mMaterialPath);
	SetShader(GetWorld.ShaderManager->BasicShader);

	if (std::filesystem::exists(InMaterialName) && std::filesystem::is_regular_file(InMaterialName))
	{
		Utils::Serialization::DeSerialize(InMaterialName.data(), this);
	}
}

JMaterial::JMaterial(JWTextView InMaterialName)
	: JMaterial(WString2String(InMaterialName.data()))
{}

uint32_t JMaterial::GetType() const
{
	return StringHash("JMaterial");
}

bool JMaterial::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// Material Path
	Utils::Serialization::Serialize_Text(mPath, FileStream);

	// Material Name
	Utils::Serialization::Serialize_Text(mMaterialName, FileStream);

	// Material Param Count
	int32_t paramCount = mMaterialParams.size();
	Utils::Serialization::Serialize_Primitive(&paramCount, sizeof(paramCount), FileStream);
	
	// Material Params
	for (int32_t i = 0; i < paramCount; ++i)
	{
		mMaterialParams[i].Serialize_Implement(FileStream);
	}
	
	// Shader File Name
	if (!mShader)
		mShader = GetWorld.ShaderManager->BasicShader;
	JWText shaderName = mShader->GetShaderFile();
	Utils::Serialization::Serialize_Text(shaderName, FileStream);

	return true;
}

bool JMaterial::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}
	
	// Material Path
	Utils::Serialization::DeSerialize_Text(mMaterialPath, InFileStream);
	
	// Material Name
	Utils::Serialization::DeSerialize_Text(mMaterialName, InFileStream);
	
	// Material Param Count
	int32_t paramCount;
	Utils::Serialization::DeSerialize_Primitive(&paramCount, sizeof(paramCount), InFileStream);
	
	// Material Params
	mMaterialParams.reserve(paramCount);
	for (int32_t i = 0; i < paramCount; ++i)
	{
		FMaterialParam param;
		param.DeSerialize_Implement(InFileStream);
		mMaterialParams.push_back(param);
	}
	
	// Shader File Name
	JWText shaderName;
	Utils::Serialization::DeSerialize_Text(shaderName, InFileStream);
	if (!shaderName.empty())
	{
		mShader = GetWorld.ShaderManager->Load<JShader>(shaderName);
	}
	else
	{
		mShader = GetWorld.ShaderManager->BasicShader;
	}

	return true;
}

void JMaterial::BindShader(ID3D11DeviceContext* InDeviceContext)
{
	assert(mShader);

	// 셰이더를 적용
	GetWorld.ShaderManager->UpdateShader(InDeviceContext, mShader);
}

void JMaterial::UpdateMaterialParams(ID3D11DeviceContext* InDeviceContext, const void* InData, uint32_t InSize)
{
	
}

void JMaterial::EditMaterialParam(const JText& InParamName, const FMaterialParam& InMaterialParam)
{
	if (FMaterialParam* origin = GetMaterialParam(InParamName))
	{
		*origin = InMaterialParam;
	}
}

const FMaterialParam* JMaterial::GetMaterialParam(const JText& InParamName) const
{
	return const_cast<JMaterial*>(this)->GetMaterialParam(InParamName);
}

// const FMaterialParam* JMaterial::GetMaterialParam(const JWText& InParamName) const
// {
// 	// const_cast로 상수성을 제거하고 비상수 버전을 호출
// 	// 코드중복을 사용하지 않기 위해...
// 	// FIXME: const_cast를 사용하지 않는 방법이 있을까? 
// 	return const_cast<JMaterial*>(this)->GetMaterialParam(InParamName);
// }

FMaterialParam* JMaterial::GetMaterialParam(const JText& InParamName)
{
	if (mMaterialParams.empty())
	{
		return nullptr;
	}

	const uint32_t hash = StringHash(InParamName.c_str());

	for (auto& param : mMaterialParams)
	{
		if (param.Key == hash)
		{
			return &param;
		}
	}

	return nullptr;
}

void JMaterial::SetShader(JShader* InShader)
{
	mShader = InShader;
}

FMaterialParam Utils::Material::CreateTextureParam(const char* ParamName, const char* FileName, int32_t Index)
{
	FMaterialParam materialParams;

	materialParams.Name           = ParamName;
	materialParams.Key            = StringHash(ParamName);
	materialParams.ParamValue     = EMaterialParamValue::Texture2D;
	materialParams.StringValue    = FileName;
	materialParams.bInstanceParam = true;
	// materialParams.TextureValue   = GetWorld.TextureManager->CreateOrLoad(FileName);

	return materialParams;
}

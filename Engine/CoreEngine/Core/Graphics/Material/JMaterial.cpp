#include "JMaterial.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Utils.h"


JMaterial::JMaterial() = default;

JMaterial::JMaterial(JTextView InMaterialName)
	: mShader(nullptr),
	  bTransparent(false)
{
	mMaterialName = {InMaterialName.begin(), InMaterialName.end()};
	mMaterialID   = StringHash(InMaterialName.data());

	if (std::filesystem::is_regular_file(InMaterialName))
	{
		std::ifstream path(InMaterialName.data(), std::ios::binary);
		JMaterial::DeSerialize(path);
	}
}

JMaterial::JMaterial(JWTextView InMaterialName)
	: mShader(nullptr),
	  bTransparent(false)
{
	mMaterialName = {InMaterialName.begin(), InMaterialName.end()};
	mMaterialID   = StringHash(InMaterialName.data());

	if (std::filesystem::is_regular_file(InMaterialName))
	{
		std::ifstream path(InMaterialName.data(), std::ios::binary);
		JMaterial::DeSerialize(path);
	}
}

void JMaterial::Serialize(std::ofstream& FileStream)
{
	// Material ID
	FileStream.write(reinterpret_cast<const char*>(&mMaterialID), sizeof(mMaterialID));

	// Material Name
	Utils::Serialization::Serialize_Text(mMaterialName, FileStream);

	// Transparent
	FileStream.write(reinterpret_cast<char*>(&bTransparent), sizeof(bTransparent));

	// Material Param Count
	int32_t paramCount = mMaterialParams.size();
	FileStream.write(reinterpret_cast<char*>(&paramCount), sizeof(paramCount));

	// Material Params
	for (int32_t i = 0; i < paramCount; ++i)
	{
		mMaterialParams[i].Serialize(FileStream);
	}

	// Shader File Name
	if (!mShader)
		mShader = IManager.ShaderManager.GetBasicShader();
	JWText shaderName = mShader->GetShaderFile();
	Utils::Serialization::Serialize_Text(shaderName, FileStream);
}

void JMaterial::DeSerialize(std::ifstream& InFileStream)
{
	// Material ID
	InFileStream.read(reinterpret_cast<char*>(&mMaterialID), sizeof(mMaterialID));

	// Material Name
	size_t nameSize;
	InFileStream.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
	JText rawString(nameSize, '0');
	InFileStream.read(reinterpret_cast<char*>(rawString.data()), nameSize);
	mMaterialName = String2WString(rawString);

	// Transparent
	InFileStream.read(reinterpret_cast<char*>(&bTransparent), sizeof(bTransparent));

	// Material Param Count
	int32_t paramCount;
	InFileStream.read(reinterpret_cast<char*>(&paramCount), sizeof(paramCount));

	// Material Params
	mMaterialParams.reserve(paramCount);
	for (int32_t i = 0; i < paramCount; ++i)
	{
		FMaterialParams param;
		param.DeSerialize(InFileStream);
		mMaterialParams.push_back(param);
	}

	// Shader File Name
	JWText shaderName;
	Utils::Serialization::DeSerialize_Text(shaderName, InFileStream);
	mShader = IManager.ShaderManager.CreateOrLoad(shaderName);

	ApplyTextures();
}


void JMaterial::AddMaterialParam(const FMaterialParams& InMaterialParam)
{
	mMaterialParams.push_back(InMaterialParam);
}

void JMaterial::BindMaterial(ID3D11DeviceContext* InDeviceContext)
{
	if (!mShader)
	{
		mShader = IManager.ShaderManager.GetBasicShader();
	}
	mShader->ApplyShader(InDeviceContext);

	for (int32_t i = 0; i < mMaterialParams.size(); ++i)
	{
		if (mMaterialParams[i].ParamType == EMaterialParamType::Texture2D)
		{
			mMaterialParams[i].TextureValue->PreRender(i);
		}
	}
}

const FMaterialParams* JMaterial::GetMaterialParam(const JText& InParamName) const
{
	return const_cast<JMaterial*>(this)->GetMaterialParam(InParamName);
}

const FMaterialParams* JMaterial::GetMaterialParam(const JWText& InParamName) const
{
	// const_cast로 상수성을 제거하고 비상수 버전을 호출
	// 코드중복을 사용하지 않기 위해...
	// FIXME: const_cast를 사용하지 않는 방법이 있을까? 
	return const_cast<JMaterial*>(this)->GetMaterialParam(InParamName);
}

FMaterialParams* JMaterial::GetMaterialParam(const JText& InParamName)
{
	if (mMaterialParams.empty())
	{
		return nullptr;
	}

	const uint32_t paramName = StringHash(InParamName.c_str());

	for (auto& param : mMaterialParams)
	{
		if (param.Key == paramName)
		{
			return &param;
		}
	}

	return nullptr;
}

FMaterialParams* JMaterial::GetMaterialParam(const JWText& InParamName)
{
	if (mMaterialParams.empty())
	{
		return nullptr;
	}

	const uint32_t paramName = StringHash(InParamName.c_str());

	for (auto& param : mMaterialParams)
	{
		if (param.Key == paramName)
		{
			return &param;
		}
	}

	return nullptr;
}


void JMaterial::ApplyTextures()
{
	for (auto& param : mMaterialParams)
	{
		if (param.ParamType == EMaterialParamType::Texture2D)
		{
			param.TextureValue = IManager.TextureManager.CreateOrLoad(param.StringValue.c_str());
		}
	}
}

FMaterialParams Utils::Material::CreateTextureParam(const char*   ParamName, const char* FileName, int32_t Index,
													EMaterialFlag Flag)
{
	FMaterialParams materialParams;

	if (Index == 0)
	{
		materialParams.Name.assign(ParamName);
	}
	else
	{
		materialParams.Name = std::format("{0}_{1}", ParamName, Index);
	}

	materialParams.Key            = StringHash(materialParams.Name.c_str());
	materialParams.ParamType      = EMaterialParamType::Texture2D;
	materialParams.StringValue    = FileName;
	materialParams.bInstanceParam = true;
	materialParams.Flags          = Flag;

	materialParams.TextureValue = IManager.TextureManager.CreateOrLoad(FileName);

	return materialParams;
}

#include "JMaterial.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Utils.h"


JMaterial::JMaterial()
	:
	  mMaterial(),
	  bTransparent(false)
{
	// mShader = IManager.ShaderManager.BasicShader;
	mShader = IManager.ShaderManager->FetchResource("Basic");

	Utils::DX::CreateBuffer(IManager.RenderManager->GetDevice(),
							D3D11_BIND_CONSTANT_BUFFER,
							nullptr,
							sizeof(CBuffer::Material),
							1,
							mMaterialBuffer.GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE);

}

JMaterial::JMaterial(JTextView InMaterialName)
	: JMaterial()
{
	mMaterialName = {InMaterialName.begin(), InMaterialName.end()};

	if (std::filesystem::is_regular_file(InMaterialName))
	{
		Utils::Serialization::DeSerialize(InMaterialName.data(), this);
	}

}

JMaterial::JMaterial(JWTextView InMaterialName)
	: JMaterial(WString2String(InMaterialName.data()))
{}

uint32_t JMaterial::GetHash() const
{
	return StringHash(mMaterialName.c_str());
}

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

	// Material ID
	Utils::Serialization::Serialize_Primitive(&mMaterialID, sizeof(mMaterialID), FileStream);

	// Material Name
	Utils::Serialization::Serialize_Text(mMaterialName, FileStream);

	// TransparentColor
	Utils::Serialization::Serialize_Primitive(&bTransparent, sizeof(bTransparent), FileStream);

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
		mShader = IManager.ShaderManager->BasicShader;
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

	// Material ID
	Utils::Serialization::DeSerialize_Primitive(&mMaterialID, sizeof(mMaterialID), InFileStream);

	// Material Name
	Utils::Serialization::DeSerialize_Text(mMaterialName, InFileStream);

	// TransparentColor
	Utils::Serialization::DeSerialize_Primitive(&bTransparent, sizeof(bTransparent), InFileStream);

	// Material Param Count
	int32_t paramCount;
	Utils::Serialization::DeSerialize_Primitive(&paramCount, sizeof(paramCount), InFileStream);

	// Material Params
	mMaterialParams.reserve(paramCount);
	for (int32_t i = 0; i < paramCount; ++i)
	{
		FMaterialParams param;
		param.DeSerialize_Implement(InFileStream);
		mMaterialParams.push_back(param);
	}

	// Shader File Name
	JWText shaderName;
	Utils::Serialization::DeSerialize_Text(shaderName, InFileStream);
	mShader = IManager.ShaderManager->CreateOrLoad(shaderName);

	ApplyTextures();

	return true;
}


void JMaterial::AddMaterialParam(const FMaterialParams& InMaterialParam)
{
	mMaterialParams.push_back(InMaterialParam);
}

void JMaterial::ApplyMaterialParams(ID3D11DeviceContext* InDeviceContext)
{
	assert(mShader);

	// 셰이더를 적용한다.
	mShader->ApplyShader(InDeviceContext);

	// 파라미터들을 셰이더에 넘겨준다.
	for (int32_t i = 0; i < mMaterialParams.size(); ++i)
	{
		FMaterialParams param      = mMaterialParams[i];
		const FVector4  colorValue = FVector4(param.Float3Value, 1.f);

		switch (param.Flags)
		{
		case EMaterialFlag::DiffuseColor:
			mMaterial.DiffuseColor = colorValue;
			if (param.ParamType == EMaterialParamType::Texture2D)
			{
				param.TextureValue->PreRender(EnumAsByte(EMaterialFlag::DiffuseColor));
				mMaterial.bUseDiffuseMap = 1;
			}
			break;
		case EMaterialFlag::EmissiveColor:
			mMaterial.EmissiveColor = colorValue;
			if (param.ParamType == EMaterialParamType::Texture2D)
			{
				param.TextureValue->PreRender(EnumAsByte(EMaterialFlag::EmissiveColor));
				mMaterial.bUseEmissiveMap = true;
			}
			break;

		case EMaterialFlag::SpecularColor:
			mMaterial.SpecularColor = colorValue;
			if (param.ParamType == EMaterialParamType::Texture2D)
			{
				param.TextureValue->PreRender(EnumAsByte(EMaterialFlag::SpecularColor));
				mMaterial.bUseSpecularMap = true;
			}
			break;
		case EMaterialFlag::ReflectionColor:
			mMaterial.ReflectionColor = colorValue;
			if (param.ParamType == EMaterialParamType::Texture2D)
			{
				param.TextureValue->PreRender(EnumAsByte(EMaterialFlag::ReflectionColor));
				mMaterial.bUseReflectionMap = true;
			}
			break;

		case EMaterialFlag::AmbientColor:
			mMaterial.AmbientColor = colorValue;
			if (param.ParamType == EMaterialParamType::Texture2D)
			{
				param.TextureValue->PreRender(EnumAsByte(EMaterialFlag::AmbientColor));
				mMaterial.bUseAmbientMap = true;
			}
			break;

		case EMaterialFlag::TransparentColor:
			mMaterial.TransparentColor = colorValue;
			break;

		case EMaterialFlag::DisplacementColor:
			mMaterial.DisplacementColor = colorValue;
			if (param.ParamType == EMaterialParamType::Texture2D)
			{
				param.TextureValue->PreRender(EnumAsByte(EMaterialFlag::DisplacementColor));
				mMaterial.bUseDisplacementMap = true;
			}
			break;

		case EMaterialFlag::NormalMap:
			mMaterial.NormalMap = colorValue;
			if (param.ParamType == EMaterialParamType::Texture2D)
			{
				param.TextureValue->PreRender(EnumAsByte(EMaterialFlag::NormalMap));
				mMaterial.bUseNormalMap = true;
			}
			break;

		case EMaterialFlag::DiffuseFactor:
			mMaterial.DiffuseFactor = param.FloatValue;
			break;

		case EMaterialFlag::EmissiveFactor:
			mMaterial.EmissiveFactor = param.FloatValue;
			break;

		case EMaterialFlag::SpecularFactor:
			mMaterial.SpecularFactor = param.FloatValue;
			break;

		case EMaterialFlag::ReflectionFactor:
			mMaterial.ReflectionFactor = param.FloatValue;
			break;

		case EMaterialFlag::AmbientFactor:
			mMaterial.AmbientFactor = param.FloatValue;
			break;

		case EMaterialFlag::TransparentFactor:
			mMaterial.TransparentFactor = param.FloatValue;
			break;

		case EMaterialFlag::DisplacementFactor:
			mMaterial.DisplacementFactor = param.FloatValue;
			break;

		case EMaterialFlag::Shininess:
			mMaterial.Shininess = param.FloatValue;
			break;

		case EMaterialFlag::Opacity:
			mMaterial.Opacity = param.FloatValue;
			break;

		default:
			break;
		}


	}
	Utils::DX::UpdateDynamicBuffer(InDeviceContext,
								   mMaterialBuffer.Get(),
								   &mMaterial,
								   sizeof(CBuffer::Material));
	InDeviceContext->PSSetConstantBuffers(4, 1, mMaterialBuffer.GetAddressOf());
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
			param.TextureValue = IManager.TextureManager->CreateOrLoad(param.StringValue.c_str());
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

	materialParams.TextureValue = IManager.TextureManager->CreateOrLoad(FileName);

	return materialParams;
}

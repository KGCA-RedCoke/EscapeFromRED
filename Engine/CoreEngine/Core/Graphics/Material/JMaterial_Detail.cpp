#include "JMaterial_Detail.h"

JMaterial_Detail::JMaterial_Detail(JTextView InName)
	: JMaterial(InName)
{
	JMaterial_Detail::InitializeParams();
}

void JMaterial_Detail::BindShader(ID3D11DeviceContext* InDeviceContext)
{
	JMaterial::BindShader(InDeviceContext);
}

void JMaterial_Detail::InitializeParams()
{
	if (mMaterialParams.empty())
	{
		mMaterialParams.clear();

		mMaterialParams.push_back(FMaterialParam("BaseColorMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("NormalMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("AOMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("RoughnessMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MetallicMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("EmissiveMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MaskMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("OpacityMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));


		mMaterialParams.push_back(FMaterialParam("BaseColor",
												 EMaterialParamValue::Float3,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("NormalColor",
												 EMaterialParamValue::Float3,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("NormalIntensity",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Emission",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("GlobalRoughness",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MaskPower",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Metallic",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MetallicConstant",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("NormalTiling",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Opacity",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Roughness",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("RoughnessMask",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Specular",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("TextureSize",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Tiling",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));

		Utils::Serialization::Serialize(NAME_MAT_DETAIL, this);
	}
}

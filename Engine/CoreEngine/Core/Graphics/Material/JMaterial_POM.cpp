#include "JMaterial_POM.h"

JMaterial_POM::JMaterial_POM(JTextView InName)
	: JMaterial(InName)
{
	JMaterial_POM::InitializeParams();
}

void JMaterial_POM::BindShader(ID3D11DeviceContext* InDeviceContext)
{
	JMaterial::BindShader(InDeviceContext);
}

void JMaterial_POM::InitializeParams()
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
		mMaterialParams.push_back(FMaterialParam("OpacityMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("HeightMap",
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
		mMaterialParams.push_back(FMaterialParam("Height Ratio",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Min Steps",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Max Steps",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Roughness",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("GlobalRoughness",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Specular",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Opacity",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));

		Utils::Serialization::Serialize(NAME_MAT_POM, this);
	}
}

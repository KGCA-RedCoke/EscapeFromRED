#include "JMaterial_Character.h"


JMaterial_Character::JMaterial_Character(JTextView InName)
	: JMaterial(InName)
{
	JMaterial_Character::InitializeParams();
}

void JMaterial_Character::BindShader(ID3D11DeviceContext* InDeviceContext)
{
	JMaterial::BindShader(InDeviceContext);
}

void JMaterial_Character::InitializeParams()
{
	if (!mMaterialParams.empty())
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
		mMaterialParams.push_back(FMaterialParam("MaskMap",
										 EMaterialParamValue::Texture2D,
										 &FVector::ZeroVector,
										 true));

		mMaterialParams.push_back(FMaterialParam("BaseColor",
												 EMaterialParamValue::Float3,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("AO",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Roughness",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Metallic",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Emissive",
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
		
		mMaterialParams.push_back(FMaterialParam("MaskColor1",
												 EMaterialParamValue::Float3,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MaskColor2",
												 EMaterialParamValue::Float3,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MaskColor3",
												 EMaterialParamValue::Float3,
												 &FVector::ZeroVector,
												 true));
		
		mMaterialName = "Material_Character";
		
		Utils::Serialization::Serialize(NAME_MAT_CHARACTER, this);
	}

}

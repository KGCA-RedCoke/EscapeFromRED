#include "JMaterial_Basic.h"

#include "Core/Graphics/ShaderStructs.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Shader/JShader.h"
#include "Core/Interface/JWorld.h"
#include "GUI/MGUIManager.h"

JMaterial_Basic::JMaterial_Basic(JTextView InName)
	: JMaterial(InName)
{
	JMaterial_Basic::InitializeParams();
}

void JMaterial_Basic::BindShader(ID3D11DeviceContext* InDeviceContext)
{
	JMaterial::BindShader(InDeviceContext);
}

void JMaterial_Basic::InitializeParams()
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
		mMaterialParams.push_back(FMaterialParam("Flags",
												 EMaterialParamValue::Integer,
												 &FVector::ZeroVector,
												 true));
		mMaterialName = "Basic";
		Utils::Serialization::Serialize(NAME_MAT_BASIC, this);
	}

}

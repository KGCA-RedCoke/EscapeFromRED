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

void JMaterial_Basic::BindMaterialPipeline(ID3D11DeviceContext*          InDeviceContext,
										   const JArray<FMaterialParam>& InInstanceParams)
{
	JMaterial::BindMaterialPipeline(InDeviceContext, InInstanceParams);

	// 텍스처맵 슬롯 바인딩
	for (int32_t i = 0; i < InInstanceParams.size(); ++i)
	{
		const FMaterialParam& param = InInstanceParams[i];

		param.BindMaterialParam(InDeviceContext, i);
	}
}

void JMaterial_Basic::InitializeParams()
{
	if (mMaterialParams.empty() || mMaterialParams.size() == 6)
	{
		mMaterialParams.clear();
		mMaterialParams.push_back(FMaterialParam(CBuffer::NAME_CONSTANT_VARIABLE_MATERIAL_DIFFUSE,
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam(CBuffer::NAME_CONSTANT_VARIABLE_MATERIAL_NORMAL,
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("AOTex",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("RoughnessTex",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MetallicTex",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam(CBuffer::NAME_CONSTANT_VARIABLE_MATERIAL_AO,
												 EMaterialParamValue::Float2,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam(CBuffer::NAME_CONSTANT_VARIABLE_MATERIAL_ROUGHNESS,
												 EMaterialParamValue::Float2,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam(CBuffer::NAME_CONSTANT_VARIABLE_MATERIAL_METALLIC,
												 EMaterialParamValue::Float2,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam(CBuffer::NAME_CONSTANT_VARIABLE_MATERIAL_USAGE_FLAG,
												 EMaterialParamValue::Integer,
												 &FVector::ZeroVector,
												 true));
	}

}

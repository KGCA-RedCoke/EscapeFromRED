#include "JMaterial_Basic.h"

#include "Core/Graphics/ShaderStructs.h"

JMaterial_Basic::JMaterial_Basic(JTextView InName)
	: JMaterial(InName)
{
	JMaterial_Basic::InitializeParams();
}

void JMaterial_Basic::InitializeParams()
{
	if (mMaterialParams.empty())
	{
		mMaterialParams.push_back(FMaterialParam(CBuffer::NAME_CONSTANT_VARIABLE_MATERIAL_DIFFUSE,
												 EMaterialParamValue::Float4,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam(CBuffer::NAME_CONSTANT_VARIABLE_MATERIAL_NORMAL,
												 EMaterialParamValue::Float4,
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

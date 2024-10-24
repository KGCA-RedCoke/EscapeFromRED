#include "JMaterial_Basic.h"

JMaterial_Basic::JMaterial_Basic(JTextView InName)
	: JMaterial(InName, sizeof(CBuffer::Material_Basic)),
	  mMaterialData()
{
	mParamData = &mMaterialData;

	JMaterial_Basic::InitializeParams();
}

void JMaterial_Basic::InitializeParams()
{
	if (mMaterialParams.empty())
	{
		mMaterialParams.push_back(FMaterialParam("DiffuseColor", EMaterialParamValue::Float3, &FVector::ZeroVector, true));
		mMaterialParams.push_back(FMaterialParam("AmbientColor", EMaterialParamValue::Float3, &FVector::ZeroVector, true));
		mMaterialParams.push_back(FMaterialParam("AmbientOcclusionColor",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.
				push_back(FMaterialParam("RoughnessColor", EMaterialParamValue::Float, &FVector::ZeroVector, true));
		mMaterialParams.push_back(FMaterialParam("MetallicColor", EMaterialParamValue::Float, &FVector::ZeroVector, true));

		mMaterialData.TextureUsageFlag = CBuffer::ETextureUsage::None;
	}

}

void JMaterial_Basic::UpdateParamData(void* InData)
{
	JMaterial::UpdateParamData(InData);

	static_cast<CBuffer::Material_Basic*>(InData);
}

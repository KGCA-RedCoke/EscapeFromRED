#include "JMaterial_Mirror.h"

#include "Core/Graphics/XD3DDevice.h"

JMaterial_Mirror::JMaterial_Mirror(JTextView InName)
	: JMaterial(InName)
{
	JMaterial_Mirror::InitializeParams();
}

void JMaterial_Mirror::BindShader(ID3D11DeviceContext* InDeviceContext)
{
	JMaterial::BindShader(InDeviceContext);

	G_Device.SetBlendState(EBlendState::AlphaBlend );
}

void JMaterial_Mirror::InitializeParams()
{
	if (mMaterialParams.empty())
	{
		mMaterialParams.clear();

		mMaterialParams.push_back(FMaterialParam("RoughnessMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));

		Utils::Serialization::Serialize(NAME_MAT_MIRROR, this);
	}
	
}

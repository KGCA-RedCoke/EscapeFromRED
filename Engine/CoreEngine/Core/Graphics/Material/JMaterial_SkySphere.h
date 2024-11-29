#pragma once
#include "JMaterial.h"

class JMaterial_SkySphere : public JMaterial
{
public:
	JMaterial_SkySphere(JTextView InName = "SkySphere");

public:
	void BindMaterialPipeline(ID3D11DeviceContext*          InDeviceContext,
							  const JArray<FMaterialParam>& InInstanceParams) override;

private:
	void InitializeParams() override;

private:
	struct
	{
		FVector4 SkyColor1;
		FVector4 SkyColor2;
		FVector4 CloudColor1;
		FVector4 CloudColor2;
		float    LightIntensity;
	} mSkyColorParams;
};

#pragma once
#include "JMaterial.h"

class JMaterial_SkySphere : public JMaterial
{
public:
	JMaterial_SkySphere(JTextView InName = "SkySphere");

public:
	void BindShader(ID3D11DeviceContext*          InDeviceContext) override;

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

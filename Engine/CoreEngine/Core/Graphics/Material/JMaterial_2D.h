#pragma once
#include "JMaterial.h"

class JMaterial_2D : public JMaterial
{
public:
	JMaterial_2D(JTextView InName);

public:
	void BindMaterialPipeline(ID3D11DeviceContext*          InDeviceContext,
							  const JArray<FMaterialParam>& InInstanceParams) override;

private:
	void InitializeParams() override;
};

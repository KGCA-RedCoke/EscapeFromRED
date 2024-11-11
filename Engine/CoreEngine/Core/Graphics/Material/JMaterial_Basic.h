#pragma once
#include "JMaterial.h"

class JMaterial_Basic : public JMaterial
{
public:
	JMaterial_Basic(JTextView InName = "Material_Basic");

public:
	void BindMaterialPipeline(ID3D11DeviceContext* InDeviceContext, const JArray<FMaterialParam>& InInstanceParams) override;
private:
	void InitializeParams() override;
};

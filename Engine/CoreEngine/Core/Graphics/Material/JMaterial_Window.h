#pragma once
#include "JMaterial.h"

class JMaterial_Window : public JMaterial
{
public:
	JMaterial_Window(JTextView InName = "Window");

public:
	void BindMaterialPipeline(ID3D11DeviceContext*          InDeviceContext,
							  const JArray<FMaterialParam>& InInstanceParams) override;

private:
	void InitializeParams() override;

};

#pragma once
#include "JMaterial.h"

class JMaterial_POM : public JMaterial
{
public:
	JMaterial_POM(JTextView InName);

public:
	void BindShader(ID3D11DeviceContext*          InDeviceContext) override;

private:
	void InitializeParams() override;
};

#pragma once
#include "JMaterial.h"

class JMaterial_Wind : public JMaterial
{
public:
	JMaterial_Wind(JTextView InName);

public:
	void BindShader(ID3D11DeviceContext*          InDeviceContext) override;

private:
	void InitializeParams() override;
};

#pragma once
#include "JMaterial.h"


class JMaterial_Character : public JMaterial
{
public:
	JMaterial_Character(JTextView InName);

public:
	void BindShader(ID3D11DeviceContext* InDeviceContext) override;

private:
	void InitializeParams() override;
};

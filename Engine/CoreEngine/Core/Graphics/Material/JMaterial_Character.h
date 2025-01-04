#pragma once
#include "JMaterial.h"
#include "Core/Graphics/ShaderStructs.h"

class JMaterial_Character : public JMaterial
{
public:
	JMaterial_Character(JTextView InName);

public:
	void BindShader(ID3D11DeviceContext* InDeviceContext) override;

private:
	void InitializeParams() override;
};

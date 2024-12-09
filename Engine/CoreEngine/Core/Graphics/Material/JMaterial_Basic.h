#pragma once
#include "JMaterial.h"
#include "Core/Graphics/ShaderStructs.h"

class JMaterial_Basic : public JMaterial
{
public:
	JMaterial_Basic(JTextView InName = "Material_Basic");

public:
	void BindShader(ID3D11DeviceContext* InDeviceContext) override;

private:
	void InitializeParams() override;
};

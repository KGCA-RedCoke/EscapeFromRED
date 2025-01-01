#pragma once
#include "Core/Graphics/Material/JMaterial.h"

class JMaterial_Mirror : public JMaterial
{
public:
	JMaterial_Mirror(JTextView InName);

public:
	void BindShader(ID3D11DeviceContext* InDeviceContext) override;

private:
	void InitializeParams() override;
};

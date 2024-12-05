#pragma once
#include "JShader.h"

class JShader_2D : public JShader
{
public:
	JShader_2D(const JText& InName);
	~JShader_2D() override = default;

public:
	void BindShaderPipeline(ID3D11DeviceContext* InDeviceContext) override;
	void UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext) override;

};

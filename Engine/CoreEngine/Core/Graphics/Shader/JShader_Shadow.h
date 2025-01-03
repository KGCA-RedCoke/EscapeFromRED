#pragma once
#include "JShader.h"

class JShader_Shadow : public JShader
{
public:
	JShader_Shadow(const JText& InName);
	~JShader_Shadow() override = default;

public:
	void BindShaderPipeline(ID3D11DeviceContext* InDeviceContext) override;
	void UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext) override;
};

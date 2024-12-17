#pragma once
#include "JShader.h"

class JShader_UI : public JShader
{
public:
	JShader_UI(const JText& InName);
	~JShader_UI() override = default;

public:
	void BindShaderPipeline(ID3D11DeviceContext* InDeviceContext) override;
	void UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext) override;

public:
	void SetTextureParams(ID3D11ShaderResourceView* InTexture, ID3D11ShaderResourceView* InOpacityTexture);

private:
	JArray<ID3D11ShaderResourceView*> mTextures;
	JArray<ID3D11ShaderResourceView*> mOpacityTextures;
};

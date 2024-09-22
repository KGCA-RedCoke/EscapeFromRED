#pragma once
#include "JShader.h"
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Graphics/ShaderStructs.h"

class SFXAAEffect : public JShader
{
public:
	SFXAAEffect(const JWText& InFileName);
	~SFXAAEffect() override;

public:
	void Initialize();
	void PostProcess_FXAA(struct FViewportData& InViewportData);

private:
	void CreateRenderTarget();

private:
	ComPtr<ID3D11RenderTargetView>   mRenderTargetView;
	ComPtr<ID3D11ShaderResourceView> mShaderResourceView;

	ComPtr<ID3D11Buffer> mScreenSizeConstBuffer;
	ComPtr<ID3D11Buffer> mScreenQuadVertexBuffer;

	Vertex::FVertexInfo_ScreenQuad mScreenQuadVertices[6];

};

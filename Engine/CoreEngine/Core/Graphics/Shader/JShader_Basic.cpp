#include "JShader_Basic.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"
#include "GUI/MGUIManager.h"

extern CBuffer::Light g_LightData;

JShader_Basic::JShader_Basic(const JText& InName)
	: JShader(NAME_SHADER_BASIC)
{
	mCachedPointLightData.Position  = FVector{-979.f, 496.0f, -4470.0};
	mCachedPointLightData.Color     = FVector(0.8f, 0.68f, 0.48f);
	mCachedPointLightData.Intensity = 0.8f;
	mCachedPointLightData.Range     = 200.0f;

}

void JShader_Basic::BindShaderPipeline(ID3D11DeviceContext* InDeviceContext)
{
	JShader::BindShaderPipeline(InDeviceContext);

	G_Device.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int32_t slots[2] = {0, 1};
	G_Device.SetSamplerState(ESamplerState::LinearWrap, slots, 2);
	G_Device.SetRasterState(GetWorld.GUIManager->IsRenderWireFrame()
								? ERasterState::WireFrame
								: ERasterState::CCW);
	G_Device.SetBlendState(EBlendState::AlphaBlend);
	G_Device.SetDepthStencilState(EDepthStencilState::DepthDefault);

}

void JShader_Basic::UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext)
{
	if (mCachedLightData.LightColor != g_LightData.LightColor || mCachedLightData.LightPos != g_LightData.LightPos)
	{
		mCachedLightData = g_LightData;
		UpdateConstantData(InDeviceContext, CBuffer::NAME_CONSTANT_BUFFER_LIGHT, &g_LightData);
	}

	UpdateConstantData(InDeviceContext, "SpotLightConstantBuffer", &mCachedPointLightData);

	// 상수버퍼 넘겨주기
	JShader::UpdateGlobalConstantBuffer(InDeviceContext);

}

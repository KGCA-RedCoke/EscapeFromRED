#include "JShader_2D.h"

#include "Core/Graphics/XD3DDevice.h"

JShader_2D::JShader_2D(const JText& InName)
	: JShader(InName)
{}

void JShader_2D::BindShaderPipeline(ID3D11DeviceContext* InDeviceContext)
{
	JShader::BindShaderPipeline(InDeviceContext);

	G_Device.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int32_t slots[1] = {0};
	G_Device.SetSamplerState(ESamplerState::LinearWrap, slots, 1);
	G_Device.SetRasterState(ERasterState::CCW);
	G_Device.SetBlendState(EBlendState::Opaque);
	G_Device.SetDepthStencilState(EDepthStencilState::DepthDefault);
}

void JShader_2D::UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext)
{
	JShader::UpdateGlobalConstantBuffer(InDeviceContext);
}

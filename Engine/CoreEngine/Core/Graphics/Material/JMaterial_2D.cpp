#include "JMaterial_2D.h"

#include "Core/Graphics/XD3DDevice.h"

JMaterial_2D::JMaterial_2D(JTextView InName)
	: JMaterial(InName) {}

void JMaterial_2D::BindShader(ID3D11DeviceContext*          InDeviceContext)
{
	JMaterial::BindShader(InDeviceContext);

	G_Device.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int32_t slots[1] = {0};
	G_Device.SetSamplerState(ESamplerState::LinearWrap, slots, 1);
	G_Device.SetRasterState(ERasterState::CCW);
	G_Device.SetBlendState(EBlendState::Opaque);
	G_Device.SetDepthStencilState(EDepthStencilState::DepthDefault);

	ID3D11ShaderResourceView* texture;

	// if (InInstanceParams[0].TextureValue != nullptr)
	// {
	// 	texture = InInstanceParams[0].TextureValue->GetSRV();
	// }

	InDeviceContext->PSSetShaderResources(0, 1, &texture);
}

void JMaterial_2D::InitializeParams()
{
	JMaterial::InitializeParams();
}

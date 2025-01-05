#include "JShader_Shadow.h"

#include "MShaderManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"

JShader_Shadow::JShader_Shadow(const JText& InName)
	: JShader(InName) {}

void JShader_Shadow::BindShaderPipeline(ID3D11DeviceContext* InDeviceContext)
{
	if (!InDeviceContext)
	{
		LOG_CORE_ERROR("DeviceContext is nullptr");
		return;
	}
	// 입력 레이아웃 설정
	InDeviceContext->IASetInputLayout(mDefaultShaderData.InputLayout.Get());

	// 셰이더 설정
	InDeviceContext->VSSetShader(mDefaultShaderData.VertexShader.Get(), nullptr, 0);
	InDeviceContext->PSSetShader(mDefaultShaderData.PixelShader.Get(), nullptr, 0);

	G_Device.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	G_Device.SetDepthStencilState(EDepthStencilState::DepthDefault);
	G_Device.SetBlendState(EBlendState::CustomAlphaBlend);
	G_Device.SetRasterState(ERasterState::SlopeScaledDepthBias);

	CBuffer::Camera lightCameraData;
	lightCameraData.View       = XMMatrixTranspose(GetWorld.DirectionalLightView);
	lightCameraData.Projection = XMMatrixTranspose(GetWorld.DirectionalLightProj);

	UpdateConstantData(InDeviceContext,
					   CBuffer::NAME_CONSTANT_BUFFER_CAMERA,
					   &lightCameraData);
	for (int32_t i = 0; i < mDefaultShaderData.ConstantBuffers.size(); ++i)
	{
		mDefaultShaderData.ConstantBuffers.at(i).SetConstantBuffer(InDeviceContext);
	}
}

void JShader_Shadow::UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext)
{
	JShader::UpdateGlobalConstantBuffer(InDeviceContext);
}

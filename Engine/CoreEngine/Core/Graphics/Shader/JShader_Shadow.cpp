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
	InDeviceContext->HSSetShader(mDefaultShaderData.HullShader.Get(), nullptr, 0);
	InDeviceContext->DSSetShader(mDefaultShaderData.DomainShader.Get(), nullptr, 0);
	InDeviceContext->GSSetShader(mDefaultShaderData.GeometryShader.Get(), nullptr, 0);
	InDeviceContext->PSSetShader(mDefaultShaderData.PixelShader.Get(), nullptr, 0);

	G_Device.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int32_t slot0 = 0;
	int32_t slot1 = 1;
	int32_t slot2 = 2;
	G_Device.SetSamplerState(ESamplerState::LinearWrap, &slot0, 1);
	G_Device.SetSamplerState(ESamplerState::LinearClamp, &slot1, 1);
	G_Device.SetSamplerState(ESamplerState::Shadow, &slot2, 1);

	G_Device.SetBlendState(EBlendState::Opaque);
	G_Device.SetDepthStencilState(EDepthStencilState::DepthDefault);

	G_Device.SetRasterState( ERasterState::CCW);

	// // 입력 레이아웃 설정
	// InDeviceContext->IASetInputLayout(mDefaultShaderData.InputLayout.Get());
	//
	// // 셰이더 설정
	// InDeviceContext->VSSetShader(mDefaultShaderData.VertexShader.Get(), nullptr, 0);
	// InDeviceContext->PSSetShader(nullptr, nullptr, 0);
	//
	// G_Device.SetDepthStencilState(EDepthStencilState::DepthDefault);
	// G_Device.SetBlendState(EBlendState::CustomAlphaBlend);
	// G_Device.SetRasterState(ERasterState::SlopeScaledDepthBias);

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

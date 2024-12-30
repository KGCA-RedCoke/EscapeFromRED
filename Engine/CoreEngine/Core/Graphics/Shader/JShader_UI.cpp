#include "JShader_UI.h"

#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/JWorld.h"

JShader_UI::JShader_UI(const JText& InName)
	: JShader(InName)
{}

void JShader_UI::BindShaderPipeline(ID3D11DeviceContext* InDeviceContext)
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
	int32_t slots = {0};
	G_Device.SetSamplerState(ESamplerState::LinearWrap, &slots, 1);
	G_Device.SetBlendState(EBlendState::Opaque);
	G_Device.SetRasterState(ERasterState::CullNone);

	// ID3D11ShaderResourceView* textureArraySRV = CreateTextureArray(device, context, mTextures.data(), mTextures.size());

	InDeviceContext->PSSetShaderResources(0, mTextures.size(), mTextures.data());
	InDeviceContext->PSSetShaderResources(64, mOpacityTextures.size(), mOpacityTextures.data());

	mTextures.clear();
	mOpacityTextures.clear();

	float    aspect = GetWorld.CameraManager->GetCurrentMainCam()->GetAspect();
	FVector2 camera(1.0f, aspect);
	UpdateConstantData(InDeviceContext, CBuffer::NAME_CONSTANT_BUFFER_VIEWPORTSCALE, &camera);

	UpdateGlobalConstantBuffer(InDeviceContext);
}

void JShader_UI::UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext)
{
	for (int32_t i = 0; i < mDefaultShaderData.ConstantBuffers.size(); ++i)
	{
		mDefaultShaderData.ConstantBuffers.at(i).SetConstantBuffer(InDeviceContext);
	}
}

void JShader_UI::SetTextureParams(ID3D11ShaderResourceView* InTexture, ID3D11ShaderResourceView* InOpacityTexture)
{
	mTextures.emplace_back(InTexture);
	mOpacityTextures.emplace_back(InOpacityTexture);
}

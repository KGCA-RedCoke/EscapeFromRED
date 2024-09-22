#include "SamplerState.h"

#include "Core/Graphics/XD3DDevice.h"


SamplerState::SamplerState(const UINT InSlot, const ESamplerType InSamplerType, const FLinearColor& InColor)
	: mSlot(InSlot), mSamplerType(InSamplerType), mColor(InColor) {}

SamplerState::~SamplerState() = default;

void SamplerState::Initialize()
{
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD         = 0;
	samplerDesc.MaxLOD         = D3D11_FLOAT32_MAX;

	memcpy(samplerDesc.BorderColor, mColor.RGBA, sizeof(FLinearColor));

	switch (mSamplerType)
	{
	case ESamplerType::PointWrap:
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case ESamplerType::LinearWrap:
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case ESamplerType::PointBorder:
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	case ESamplerType::LinearBorder:
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	}


	CheckResult(DeviceRSC.GetDevice()->CreateSamplerState(&samplerDesc, mSampler.GetAddressOf()));

}

void SamplerState::Update(float DeltaTime) {}

void SamplerState::Render()
{
	DeviceRSC.GetImmediateDeviceContext()->PSSetSamplers(mSlot, 1, mSampler.GetAddressOf());
}

void SamplerState::Release()
{
	mSampler = nullptr;
}

#include "SFXAAEffect.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Utils/Graphics/DXUtils.h"

SFXAAEffect::SFXAAEffect(const JWText& InFileName)
	: JShader(InFileName)
{
	Initialize();
}

SFXAAEffect::~SFXAAEffect()
{
	mRenderTargetView.Reset();
	mShaderResourceView.Reset();
	mScreenSizeConstBuffer.Reset();
	mScreenQuadVertexBuffer.Reset();
}

void SFXAAEffect::Initialize()
{
	FVector4 ScreenSize = FVector4(1920, 1080, 0, 0);

	Utils::DX::CreateBuffer(
							DeviceRSC.GetDevice(),
							D3D11_BIND_CONSTANT_BUFFER,
							reinterpret_cast<void**>(&ScreenSize),
							sizeof(FVector4),
							1,
							mScreenSizeConstBuffer.GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE
						   );

	mScreenQuadVertices[0] = {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}; // 좌상
	mScreenQuadVertices[1] = {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}; // 좌하
	mScreenQuadVertices[2] = {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}; // 우하
	mScreenQuadVertices[3] = {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}; // 좌상
	mScreenQuadVertices[4] = {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}; // 우하
	mScreenQuadVertices[5] = {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}; // 우상

	Utils::DX::CreateBuffer(
							DeviceRSC.GetDevice(),
							D3D11_BIND_VERTEX_BUFFER,
							reinterpret_cast<void**>(&mScreenQuadVertices),
							sizeof(Vertex::FVertexInfo_ScreenQuad),
							6,
							mScreenQuadVertexBuffer.GetAddressOf()
						   );

	CreateRenderTarget();
}

void SFXAAEffect::PostProcess_FXAA(FViewportData& InViewportData)
{
	if (!InViewportData.RTV.Get())
	{
		LOG_CORE_ERROR("SRV is nullptr");
		return;
	}

	auto deviceContext = DeviceRSC.GetImmediateDeviceContext();
	auto commonStates  = DeviceRSC.GetDXTKCommonStates();
	assert(deviceContext);

	// 도화지(Render Target View) 한장 더 꺼내자 (여기에 Anti Aliasing을 적용할 것이다)
	deviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	uint32_t stride = sizeof(Vertex::FVertexInfo_ScreenQuad);
	uint32_t offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, mScreenQuadVertexBuffer.GetAddressOf(), &stride, &offset);

	deviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);

	// FXAA 적용 (셰이더)
	deviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);

	// 셰이더에 텍스처를 넘겨준다
	deviceContext->PSSetShaderResources(0, 1, InViewportData.SRV.GetAddressOf());

	deviceContext->PSSetConstantBuffers(0, 1, mScreenSizeConstBuffer.GetAddressOf());

	auto sampler = commonStates->LinearWrap();
	deviceContext->PSSetSamplers(0, 1, &sampler);

	deviceContext->Draw(6, 0);

	
}

void SFXAAEffect::CreateRenderTarget()
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width            = 1920;
	desc.Height           = 1080;
	desc.MipLevels        = 1;
	desc.ArraySize        = 1;
	desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage            = D3D11_USAGE_DEFAULT;
	desc.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	ComPtr<ID3D11Texture2D> rtvTexture;


	CheckResult(DeviceRSC.GetDevice()->CreateTexture2D(&desc, nullptr, rtvTexture.GetAddressOf()));
	CheckResult(DeviceRSC.GetDevice()->CreateRenderTargetView(rtvTexture.Get(),
															  nullptr,
															  mRenderTargetView.GetAddressOf()));
	CheckResult(DeviceRSC.GetDevice()->CreateShaderResourceView(rtvTexture.Get(),
																nullptr,
																mShaderResourceView.GetAddressOf()));

	rtvTexture.Reset();
}

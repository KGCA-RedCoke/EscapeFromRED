#include "common_pch.h"
#include "MViewportManager.h"
#include <ranges>

#include "Core/Graphics/GraphicDevice.h"
#include "Core/Utils/Utils.h"


MViewportManager::MViewportManager()
{}

MViewportManager::~MViewportManager()
{
	// for (auto& viewportData : mViewportRTVs | std::views::values)
	// {
	// 	viewportData.RTV = nullptr;
	// 	viewportData.SRV = nullptr;
	// }
}

FViewportData::FViewportData(const JText& InName, uint32_t InWidth, uint32_t InHeight)
{
	Hash = StringHash(InName.c_str());

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width            = InWidth;
	desc.Height           = InHeight;
	desc.MipLevels        = 1;
	desc.ArraySize        = 1;
	desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage            = D3D11_USAGE_DEFAULT;
	desc.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	ViewportDesc.Width    = static_cast<FLOAT>(InWidth);
	ViewportDesc.Height   = static_cast<FLOAT>(InHeight);
	ViewportDesc.MinDepth = 0.0f;
	ViewportDesc.MaxDepth = 1.0f;
	ViewportDesc.TopLeftX = 0.0f;
	ViewportDesc.TopLeftY = 0.0f;

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width              = InWidth;
	depthStencilDesc.Height             = InHeight;
	depthStencilDesc.MipLevels          = 1;
	depthStencilDesc.ArraySize          = 1;
	depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags     = 0;
	depthStencilDesc.MiscFlags          = 0;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	ComPtr<ID3D11Texture2D> texBuffer;
	ComPtr<IDXGISurface>    dxgiBackBuffer;
	ComPtr<ID3D11Texture2D> depthBuffer;

	CheckResult(G_Context.GetDevice()->CreateTexture2D(&desc, nullptr, texBuffer.GetAddressOf()));
	CheckResult(G_Context.GetDevice()->CreateTexture2D(&depthStencilDesc, nullptr, depthBuffer.GetAddressOf()));

	CheckResult(G_Context.GetDevice()->CreateRenderTargetView(texBuffer.Get(), nullptr, RTV.GetAddressOf()));
	CheckResult(G_Context.GetDevice()->CreateDepthStencilView(depthBuffer.Get(), &depthStencilViewDesc, DepthStencilView.GetAddressOf()));
	CheckResult(G_Context.GetDevice()->CreateShaderResourceView(texBuffer.Get(), nullptr, SRV.GetAddressOf()));

	// 2D Side RenderTarget (DWrite)

	D2D1_RENDER_TARGET_PROPERTIES props;
	{
		props.type        = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
		props.dpiX        = 96.f;
		props.dpiY        = 96.f;
		props.usage       = D2D1_RENDER_TARGET_USAGE_NONE;
		props.minLevel    = D2D1_FEATURE_LEVEL_DEFAULT;
	}
	CheckResult(
				texBuffer->QueryInterface( // Query Interface
										  __uuidof(IDXGISurface), // 내가 원하는 자료형(ID)
										  reinterpret_cast<void**>(dxgiBackBuffer.GetAddressOf() // 담아질 개체
										  )));
	CheckResult(G_Context.Get2DFactory()->
						  CreateDxgiSurfaceRenderTarget(
														dxgiBackBuffer.Get(),
														&props,
														RTV_2D.GetAddressOf()));

	texBuffer      = nullptr;
	dxgiBackBuffer = nullptr;
	depthBuffer    = nullptr;
}

void MViewportManager::Initialize()
{
	CreateOrLoad(Name_Editor_Viewport, 1920, 1080);
}

void MViewportManager::ResizeViewport(JTextView InViewportName,
									  uint32_t  InWidth, uint32_t InHeight,
									  float     InOriginX, float  InOriginY)
{
	uint32_t stringHash = StringHash(InViewportName.data());

	assert(mManagedList.contains(stringHash), "Invalid viewport ID");

	mManagedList[stringHash]->ViewportDesc.TopLeftX = InOriginX;
	mManagedList[stringHash]->ViewportDesc.TopLeftY = InOriginY;
	mManagedList[stringHash]->ViewportDesc.Width    = InWidth;
	mManagedList[stringHash]->ViewportDesc.Height   = InHeight;
}

void MViewportManager::SetRenderTarget(JTextView InViewportName, const FLinearColor& InClearColor)
{
	uint32_t stringHash = StringHash(InViewportName.data());

	assert(mManagedList.contains(stringHash), "Invalid viewport ID");

	G_Context.GetImmediateDeviceContext()->
			  ClearDepthStencilView(mManagedList[stringHash]->DepthStencilView.Get(),
									D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
									1.0f,
									0);

	// RenderTarget 영역 설정 
	G_Context.GetImmediateDeviceContext()->
			  OMSetRenderTargets(
								 1,
								 mManagedList[stringHash]->RTV.GetAddressOf(),
								 mManagedList[stringHash]->DepthStencilView.Get());

	// 새 화면으로 Clear
	G_Context.GetImmediateDeviceContext()->
			  ClearRenderTargetView(
									mManagedList[stringHash]->RTV.Get(),
									InClearColor.RGBA);

	// 각 뷰표트의 영역에 맞게 조절
	G_Context.GetImmediateDeviceContext()->
			  RSSetViewports(
							 1,
							 &mManagedList[stringHash]->ViewportDesc);

}

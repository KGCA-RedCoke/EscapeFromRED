#include "MViewportManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/Utils.h"
#include "Core/Utils/Graphics/DXUtils.h"

FViewportData::FViewportData(const JText& InName, uint32_t InWidth, uint32_t InHeight, DXGI_FORMAT InDepthStencilFormat)
	: DepthStencilFormat(InDepthStencilFormat)
{
	Hash = StringHash(InName.c_str());

	auto device = GetWorld.D3D11API->GetDevice();

	// ------------------------- RenderTarget -------------------------
	ComPtr<ID3D11Texture2D> texBuffer;
	Utils::DX::CreateRenderTarget(device,
								  InWidth,
								  InHeight,
								  RTV.ReleaseAndGetAddressOf(),
								  SRV.ReleaseAndGetAddressOf(),
								  texBuffer.ReleaseAndGetAddressOf());

	// ------------------------- Viewport -------------------------
	Utils::DX::CreateViewport(InWidth, InHeight, &ViewportDesc);

	// ------------------------- DepthBuffer & Depth Stencil View -------------------------
	ComPtr<ID3D11Texture2D> depthBuffer;
	Utils::DX::CreateDepthStencilView(
									  device,
									  InWidth,
									  InHeight,
									  DepthStencilView.ReleaseAndGetAddressOf(),
									  depthBuffer.GetAddressOf(),
									  DepthSRV.ReleaseAndGetAddressOf(),
									  DepthStencilFormat);

	// ------------------------- Depth Stencil State -------------------------
	Utils::DX::CreateDepthStencilState(device,
									   true,
									   D3D11_DEPTH_WRITE_MASK_ALL,
									   D3D11_COMPARISON_LESS_EQUAL,
									   DepthStencilState.ReleaseAndGetAddressOf());


	// 2D Side RenderTarget (DWrite)
	Utils::DX::CreateD2DRenderTarget(GetWorld.D3D11API->Get2DFactory(),
									 texBuffer.Get(),
									 RTV_2D.GetAddressOf());

	texBuffer   = nullptr;
	depthBuffer = nullptr;

	// OnViewportResized.Bind([&](uint32_t width, uint32_t height){
	// 	Resize(device, width, height);
	// });
}

FViewportData::FViewportData(const JText& InName, const FVector2& InSize)
	: FViewportData(InName, InSize.x, InSize.y) {}

MViewportManager::MViewportManager()
{
	Initialize_Internal();
}

void FViewportData::Resize(ID3D11Device* InDevice, uint32_t InWidth, uint32_t InHeight)
{
	// 1. 기존 뷰 해제
	RTV               = nullptr;
	SRV               = nullptr;
	RTV_2D            = nullptr;
	DepthStencilView  = nullptr;
	DepthStencilState = nullptr;

	Create(InDevice, InWidth, InHeight);

	OnViewportResized.Execute(InWidth, InHeight);
}

void FViewportData::Create(ID3D11Device* InDevice, uint32_t InWidth, uint32_t InHeight)
{
	// ------------------------- RenderTarget -------------------------
	ComPtr<ID3D11Texture2D> texBuffer;
	Utils::DX::CreateRenderTarget(InDevice,
								  InWidth,
								  InHeight,
								  RTV.ReleaseAndGetAddressOf(),
								  SRV.ReleaseAndGetAddressOf(),
								  texBuffer.ReleaseAndGetAddressOf());

	// ------------------------- Viewport -------------------------
	Utils::DX::CreateViewport(InWidth, InHeight, &ViewportDesc);

	// ------------------------- DepthBuffer & Depth Stencil View ------------------	-------
	ComPtr<ID3D11Texture2D> depthBuffer;
	Utils::DX::CreateDepthStencilView(
									  InDevice,
									  InWidth,
									  InHeight,
									  DepthStencilView.ReleaseAndGetAddressOf(),
									  depthBuffer.GetAddressOf(),
									  DepthSRV.ReleaseAndGetAddressOf(),
									  DepthStencilFormat);

	// ------------------------- Depth Stencil State -------------------------
	Utils::DX::CreateDepthStencilState(InDevice,
									   true,
									   D3D11_DEPTH_WRITE_MASK_ALL,
									   D3D11_COMPARISON_LESS_EQUAL,
									   DepthStencilState.ReleaseAndGetAddressOf());


	// 2D Side RenderTarget (DWrite)
	Utils::DX::CreateD2DRenderTarget(GetWorld.D3D11API->Get2DFactory(),
									 texBuffer.Get(),
									 RTV_2D.ReleaseAndGetAddressOf());

	texBuffer   = nullptr;
	depthBuffer = nullptr;
}

void MViewportManager::Initialize_Internal()
{
	Load(Name_Editor_Viewport, 1280, 720);
	Load(Name_Editor_Viewport_Top, 1280, 720);
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
	uint32_t stringHash = StringHash(ParseFile(InViewportName.data()).c_str());

	assert(mManagedList.contains(stringHash), "Invalid viewport ID");

	ID3D11ShaderResourceView* nullSRV[16]{nullptr};
	GetWorld.D3D11API->GetImmediateDeviceContext()->
			 PSSetShaderResources(0, 16, nullSRV);

	// 새 화면으로 Clear
	GetWorld.D3D11API->GetImmediateDeviceContext()->
			 ClearRenderTargetView(
								   mManagedList[stringHash]->RTV.Get(),
								   InClearColor.RGBA);

	GetWorld.D3D11API->GetImmediateDeviceContext()->
			 ClearDepthStencilView(mManagedList[stringHash]->DepthStencilView.Get(),
								   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
								   1.f,
								   0);

	// RenderTarget 영역 설정 
	GetWorld.D3D11API->GetImmediateDeviceContext()->
			 OMSetRenderTargets(
								1,
								mManagedList[stringHash]->RTV.GetAddressOf(),
								mManagedList[stringHash]->DepthStencilView.Get());

	// 각 뷰표트의 영역에 맞게 조절
	G_Device.GetImmediateDeviceContext()->
			 RSSetViewports(
							1,
							&mManagedList[stringHash]->ViewportDesc);

}

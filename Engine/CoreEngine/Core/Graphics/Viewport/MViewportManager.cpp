#include "MViewportManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Utils.h"
#include "Core/Utils/Graphics/DXUtils.h"

FViewportData::FViewportData(const JText& InName, uint32_t InWidth, uint32_t InHeight)
{
	Hash = StringHash(InName.c_str());

	auto device = IManager.RenderManager->GetDevice();

	// ------------------------- RenderTarget -------------------------
	ComPtr<ID3D11Texture2D> texBuffer;
	Utils::DX::CreateRenderTarget(device,
								  InWidth,
								  InHeight,
								  RTV.GetAddressOf(),
								  SRV.GetAddressOf(),
								  texBuffer.GetAddressOf());

	// ------------------------- Viewport -------------------------
	Utils::DX::CreateViewport(InWidth, InHeight, &ViewportDesc);

	// ------------------------- DepthBuffer & Depth Stencil View -------------------------
	ComPtr<ID3D11Texture2D> depthBuffer;
	Utils::DX::CreateDepthStencilView(
									  device,
									  InWidth,
									  InHeight,
									  DepthStencilView.GetAddressOf(),
									  depthBuffer.GetAddressOf());

	// ------------------------- Depth Stencil State -------------------------
	Utils::DX::CreateDepthStencilState(device,
									   true,
									   D3D11_DEPTH_WRITE_MASK_ALL,
									   D3D11_COMPARISON_LESS_EQUAL,
									   DepthStencilState.GetAddressOf());


	// 2D Side RenderTarget (DWrite)
	Utils::DX::CreateD2DRenderTarget(IManager.RenderManager->Get2DFactory(),
									 texBuffer.Get(),
									 RTV_2D.GetAddressOf());

	texBuffer   = nullptr;
	depthBuffer = nullptr;
}

MViewportManager::MViewportManager()
{
	Initialize_Internal();
}

void MViewportManager::Initialize_Internal()
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

	// ID3D11ShaderResourceView* nullSRV = nullptr;
	// Renderer.GetImmediateDeviceContext()->
	// 		  PSSetShaderResources(0, 1, &nullSRV);

	IManager.RenderManager->GetImmediateDeviceContext()->
			 ClearDepthStencilView(mManagedList[stringHash]->DepthStencilView.Get(),
								   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
								   1.0f,
								   0);

	// RenderTarget 영역 설정 
	IManager.RenderManager->GetImmediateDeviceContext()->
			 OMSetRenderTargets(
								1,
								mManagedList[stringHash]->RTV.GetAddressOf(),
								mManagedList[stringHash]->DepthStencilView.Get());

	// 새 화면으로 Clear
	IManager.RenderManager->GetImmediateDeviceContext()->
			 ClearRenderTargetView(
								   mManagedList[stringHash]->RTV.Get(),
								   InClearColor.RGBA);

	// 각 뷰표트의 영역에 맞게 조절
	Renderer.GetImmediateDeviceContext()->
			 RSSetViewports(
							1,
							&mManagedList[stringHash]->ViewportDesc);

}

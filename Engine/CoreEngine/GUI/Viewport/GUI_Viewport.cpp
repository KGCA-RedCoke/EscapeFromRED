#include "GUI_Viewport.h"

#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/Graphics/DXUtils.h"
#include "GUI/GUI_Inspector.h"
#include "GUI/MGUIManager.h"


GUI_Viewport::GUI_Viewport(const JText& InTitle)
	: GUI_Base(InTitle),
	  bIsFocused(false),
	  bIsHovered(false),
	  mEditorCameraRef(nullptr),
	  mViewportTitle(InTitle)
{
	GUI_Viewport::Initialize();
}

void GUI_Viewport::Initialize()
{
	if (!mViewportData)
	{
		mViewportData = GetWorld.ViewportManager->FetchResource(mViewportTitle);
	}
	mViewportData->OnViewportResized.Bind([&](uint32_t InWidth, uint32_t InHeight){
		if (mEditorCameraRef)
		{
			mEditorCameraRef->SetProjParams(M_PI / 3.f,
											static_cast<float>(InWidth) / static_cast<float>(InHeight),
											10.f,
											100000.f);
		}
	});
}

void GUI_Viewport::Update_Implementation(float DeltaTime)
{
	bIsFocused = ImGui::IsWindowFocused();
	bIsHovered = ImGui::IsWindowHovered();

	if (!mViewportData)
	{
		ImGui::End();
		LOG_CORE_FATAL("Invalid Viewport");
	}

	ImVec2 size = ImGui::GetContentRegionAvail();
	if (size.x != mCachedViewportWidth || size.y != mCachedViewportHeight)
	{
		mCachedViewportWidth  = size.x;
		mCachedViewportHeight = size.y;
		mViewportData->Resize(GetWorld.D3D11API->GetDevice(), (uint32_t)size.x, (uint32_t)size.y);
		CreateMousePickingBuffer(GetWorld.D3D11API->GetDevice());
	}

	ImGui::Image(mViewportData->SRV.Get(), ImGui::GetContentRegionAvail());

	if ((ImGui::IsItemHovered() || ImGui::IsItemFocused()) && !GetWorld.bGameMode)
	{
		if (mEditorCameraRef)
			mEditorCameraRef->Tick(DeltaTime);

		if (ImGui::IsItemClicked(0))
		{
			ImVec2 mousePos = ImGui::GetMousePos();
			mousePos.x -= ImGui::GetItemRectMin().x;
			mousePos.y -= ImGui::GetItemRectMin().y;
			UpdateMousePickingBuffer(GetWorld.D3D11API->GetImmediateDeviceContext());
			// Draw ID to Color Buffer
			auto inspector    = GetWorld.GUIManager->GetInspector();
			auto currentLevel = GetWorld.LevelManager->GetActiveLevel();

			for (auto& actor : currentLevel->mActors)
			{
				if (auto ptr = actor.get())
				{
					ptr->DrawID((ptr->GetHash()));
				}
			}

			uint32_t id = GetMousePickingValue(GetWorld.D3D11API->GetImmediateDeviceContext(), mousePos);

			for (auto& actor : currentLevel->mActors)
			{
				auto ptr = actor.get();
				if (ptr && ptr->GetHash() == id)
				{
					LOG_CORE_INFO("Selected Actor : {0}", ptr->GetName());

					inspector->SetSelectedActor(ptr);
				}
			}
		}
	}


}

void GUI_Viewport::CreateMousePickingBuffer(ID3D11Device* InDevice)
{
	// 렌더 타겟 초기화
	mMousePickingBuffer.ColorIDBuffer = nullptr;
	mMousePickingBuffer.StagingBuffer = nullptr;
	mMousePickingBuffer.RTV           = nullptr;
	mMousePickingBuffer.DepthBuffer   = nullptr;
	mMousePickingBuffer.DepthState    = nullptr;

	D3D11_TEXTURE2D_DESC stagingBufferDesc;
	ZeroMemory(&stagingBufferDesc, sizeof(stagingBufferDesc));
	stagingBufferDesc.Width              = 1;
	stagingBufferDesc.Height             = 1;
	stagingBufferDesc.MipLevels          = 1;
	stagingBufferDesc.ArraySize          = 1;
	stagingBufferDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	stagingBufferDesc.SampleDesc.Count   = 1;
	stagingBufferDesc.SampleDesc.Quality = 0;
	stagingBufferDesc.Usage              = D3D11_USAGE_STAGING;
	stagingBufferDesc.BindFlags          = 0;
	stagingBufferDesc.CPUAccessFlags     = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	stagingBufferDesc.MiscFlags          = 0;
	CheckResult(InDevice->CreateTexture2D(&stagingBufferDesc, nullptr, mMousePickingBuffer.StagingBuffer.GetAddressOf()));

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width              = mCachedViewportWidth;
	textureDesc.Height             = mCachedViewportHeight;
	textureDesc.MipLevels          = 1;
	textureDesc.ArraySize          = 1;
	textureDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count   = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage              = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags          = D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags     = 0;
	textureDesc.MiscFlags          = 0;

	// ------------------------- DepthBuffer & Depth Stencil View -------------------------
	ComPtr<ID3D11Texture2D> depthBuffer;
	Utils::DX::CreateDepthStencilView(
									  InDevice,
									  mCachedViewportWidth,
									  mCachedViewportHeight,
									  mMousePickingBuffer.DepthBuffer.GetAddressOf(),
									  depthBuffer.GetAddressOf());

	// ------------------------- Depth Stencil State -------------------------
	Utils::DX::CreateDepthStencilState(InDevice,
									   true,
									   D3D11_DEPTH_WRITE_MASK_ALL,
									   D3D11_COMPARISON_LESS_EQUAL,
									   mMousePickingBuffer.DepthState.GetAddressOf());


	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format             = textureDesc.Format;
	rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	CheckResult(InDevice->CreateTexture2D(&textureDesc, nullptr, mMousePickingBuffer.ColorIDBuffer.GetAddressOf()));
	CheckResult(InDevice->CreateRenderTargetView(mMousePickingBuffer.ColorIDBuffer.Get(),
												 &rtvDesc,
												 mMousePickingBuffer.RTV.GetAddressOf()));

	depthBuffer = nullptr;

}

void GUI_Viewport::UpdateMousePickingBuffer(ID3D11DeviceContext* InDeviceContext)
{

	// 렌더 타겟 초기화
	InDeviceContext->ClearDepthStencilView(mMousePickingBuffer.DepthBuffer.Get(),
										   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
										   1.0f,
										   0);
	InDeviceContext->OMSetRenderTargets(1, mMousePickingBuffer.RTV.GetAddressOf(), mMousePickingBuffer.DepthBuffer.Get());
	InDeviceContext->ClearRenderTargetView(mMousePickingBuffer.RTV.Get(), FLinearColor::TrueBlack.RGBA);

	G_Device.SetDepthStencilState(EDepthStencilState::DepthDefault);
}

uint32_t GUI_Viewport::GetMousePickingValue(ID3D11DeviceContext* InDeviceContext, const ImVec2& InMousePos)
{
	// 다시 렌더 타겟 초기화
	InDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	// 특정 픽셀 복사
	D3D11_BOX box;
	box.left   = static_cast<UINT>(InMousePos.x);
	box.right  = static_cast<UINT>(InMousePos.x) + 1;
	box.top    = static_cast<UINT>(InMousePos.y);
	box.bottom = static_cast<UINT>(InMousePos.y) + 1;
	box.front  = 0;
	box.back   = 1;
	InDeviceContext->CopySubresourceRegion(mMousePickingBuffer.StagingBuffer.Get(),
										   0,
										   0,
										   0,
										   0,
										   mMousePickingBuffer.ColorIDBuffer.Get(),
										   0,
										   &box);

	// 픽셀 읽기
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	CheckResult(InDeviceContext->Map(mMousePickingBuffer.StagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource));

	if (mappedResource.pData)
	{
		uint8_t* data = static_cast<uint8_t*>(mappedResource.pData);
		float    r    = data[0] / 255.0f;
		float    g    = data[1] / 255.0f;
		float    b    = data[2] / 255.0f;
		float    a    = data[3] / 255.0f;

		uint32_t hash = Color2Hash({r, g, b, a});

		InDeviceContext->Unmap(mMousePickingBuffer.StagingBuffer.Get(), 0);
		return hash;

	}
	return 0;
}

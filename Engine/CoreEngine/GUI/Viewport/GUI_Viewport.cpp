#include "GUI_Viewport.h"

#include "Core/Entity/Actor/JActor.h"
#include "Core/Entity/Camera/JCamera.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/MManagerInterface.h"
#include "GUI/GUI_Inspector.h"


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
		mViewportData = IManager.ViewportManager->FetchResource(mViewportTitle);
	}
	mViewportData->OnViewportResized.Bind([&](uint32_t InWidth, uint32_t InHeight){
		if (mEditorCameraRef)
		{
			mEditorCameraRef->SetProjParams(M_PI / 4,
											static_cast<float>(InWidth) / static_cast<float>(InHeight),
											0.1f,
											1000000.f);
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
		mViewportData->Resize(IManager.RenderManager->GetDevice(), size.x, size.y);
		CreateMousePickingBuffer(IManager.RenderManager->GetDevice());
	}

	ImGui::Image(mViewportData->SRV.Get(), ImGui::GetContentRegionAvail());

	if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
	{
		if (mEditorCameraRef)
			mEditorCameraRef->Update(DeltaTime);

		if (ImGui::IsItemClicked(0))
		{
			ImVec2 mousePos = ImGui::GetMousePos();
			mousePos.x -= ImGui::GetItemRectMin().x;
			mousePos.y -= ImGui::GetItemRectMin().y;
			UpdateMousePickingBuffer(IManager.RenderManager->GetImmediateDeviceContext());
			// Draw ID to Color Buffer
			auto inspector = IManager.GUIManager->GetInspector();
			auto actors    = inspector->GetSelectedSceneComponent();
			for (auto& actor : actors)
			{
				auto ptr = actor.second.lock();
				if (ptr)
				{
					ptr->DrawID((ptr->GetHash()));
				}
			}

			uint32_t    id    = GetMousePickingValue(IManager.RenderManager->GetImmediateDeviceContext(), mousePos);
			// const float depth = Utils::DX::GetDepthValue(IManager.RenderManager->GetDevice(),
			// 											 IManager.RenderManager->GetImmediateDeviceContext(),
			// 											 mViewportData->DepthStencilView.Get(),
			// 											 FVector2(mousePos.x, mousePos.y));
			// FVector worldPos = Utils::DX::Screen2World(FVector2{mousePos.x, mousePos.y},
			// 										   FVector2{
			// 											   size.x,
			// 											   size.y
			// 										   },
			// 										   mEditorCameraRef->GetViewMatrix(),
			// 										   mEditorCameraRef->GetProjMatrix(),
			// 										   depth);

			// LOG_CORE_INFO("World Position : {0}, {1}, {2}", worldPos.x, worldPos.y, worldPos.z);

			for (auto& actor : actors)
			{
				auto ptr = actor.second.lock();
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

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format             = textureDesc.Format;
	rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	CheckResult(InDevice->CreateTexture2D(&textureDesc, nullptr, mMousePickingBuffer.ColorIDBuffer.GetAddressOf()));
	CheckResult(InDevice->CreateRenderTargetView(mMousePickingBuffer.ColorIDBuffer.Get(),
												 &rtvDesc,
												 mMousePickingBuffer.RTV.GetAddressOf()));

}

void GUI_Viewport::UpdateMousePickingBuffer(ID3D11DeviceContext* InDeviceContext)
{

	// 렌더 타겟 초기화
	InDeviceContext->OMSetRenderTargets(1, mMousePickingBuffer.RTV.GetAddressOf(), nullptr);
	InDeviceContext->ClearRenderTargetView(mMousePickingBuffer.RTV.Get(), FLinearColor::TrueBlack.RGBA);
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

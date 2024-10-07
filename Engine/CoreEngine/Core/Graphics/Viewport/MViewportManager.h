#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Manager/Manager_Base.h"
#include "Core/Utils/Math/Color.h"

constexpr char Name_Editor_Viewport[] = "Editor Viewport";

struct FViewportData
{
public:
	uint32_t                         Hash;
	ComPtr<ID3D11RenderTargetView>   RTV;			// 렌더 타겟
	ComPtr<ID3D11ShaderResourceView> SRV;			// 이미지
	ComPtr<ID2D1RenderTarget>        RTV_2D;		// DWrite Draw RTV
	ComPtr<ID3D11DepthStencilView>   DepthStencilView;			// 깊이 스텐실
	ComPtr<ID3D11DepthStencilState>  DepthStencilState;			// 깊이 스텐실 상태
	D3D11_VIEWPORT                   ViewportDesc;	// 뷰포트 정보

public:
	explicit FViewportData(const std::string& InName, uint32_t InWidth, uint32_t InHeight);
	~FViewportData() = default;
};

class MViewportManager : public Manager_Base<FViewportData, MViewportManager>
{
private:
	void Initialize_Internal();

public:
	void ResizeViewport(JTextView InViewportName, uint32_t InWidth, uint32_t InHeight, float InOriginX, float InOriginY);
	void SetRenderTarget(JTextView InViewportName, const FLinearColor& InClearColor = FLinearColor::GreenPea);

private:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MViewportManager>;
	friend class MManagerInterface;
	MViewportManager();
	~MViewportManager() = default;

public:
	MViewportManager(const MViewportManager&)            = delete;
	MViewportManager& operator=(const MViewportManager&) = delete;

#pragma endregion

};

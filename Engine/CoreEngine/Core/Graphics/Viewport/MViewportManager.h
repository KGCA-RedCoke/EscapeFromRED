#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Manager/Manager_Base.h"
#include "Core/Utils/Math/Color.h"

DECLARE_DYNAMIC_DELEGATE(FOnViewportResized, uint32_t, uint32_t);

constexpr const char* Name_Editor_Viewport     = "Editor Viewport";
constexpr const char* Name_Editor_Viewport_Top = "Editor Viewport Top";

/**
 * 뷰포트
 * 새로운 창을 만든다거나, 렌더 타겟을 만들어서 렌더링을 할 수 있게 해준다.
 * 게임 화면에 렌더링 될 화면을 이미지로 저장 -> SRV에 저장된다.
 *
 * IMGUI창에 렌더링을 하려면 ImGui::Image()에 인자로 SRV를 넘겨주면 된다.
 */
struct FViewportData
{
public:
	FOnViewportResized OnViewportResized;

public:
	uint32_t                         Hash;
	ComPtr<ID3D11RenderTargetView>   RTV;						// 렌더 타겟
	ComPtr<ID3D11ShaderResourceView> SRV;						// 이미지
	ComPtr<ID2D1RenderTarget>        RTV_2D;					// DWrite Draw RTV (2D 텍스트를 그리기 위한 RTV)
	ComPtr<ID3D11DepthStencilView>   DepthStencilView;			// 깊이 스텐실
	ComPtr<ID3D11DepthStencilState>  DepthStencilState;			// 깊이 스텐실 상태
	D3D11_VIEWPORT                   ViewportDesc;				// 뷰포트 정보

public:
	explicit FViewportData(const JText& InName, uint32_t InWidth, uint32_t InHeight);
	explicit FViewportData(const JText& InName, const FVector2& InSize);
	~FViewportData() = default;

public:
	void Resize(uint32_t InWidth, uint32_t InHeight, float InOriginX, float InOriginY);
public:
	void Resize(ID3D11Device* InDevice, uint32_t InWidth, uint32_t InHeight);

private:
	void Create(ID3D11Device* InDevice, uint32_t InWidth, uint32_t InHeight);
};

class MViewportManager : public Manager_Base<FViewportData, MViewportManager>
{
private:
	void Initialize_Internal();

public:
	void ResizeViewport(JTextView InViewportName, uint32_t InWidth, uint32_t InHeight, float InOriginX, float InOriginY);
	void SetRenderTarget(JTextView InViewportName, const FLinearColor& InClearColor = FLinearColor::Blender_Grid_Gray);

private:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MViewportManager>;
	friend class JWorld;
	MViewportManager();
	~MViewportManager() = default;

public:
	MViewportManager(const MViewportManager&)            = delete;
	MViewportManager& operator=(const MViewportManager&) = delete;

#pragma endregion

};

#pragma once
#include <directxtk/CommonStates.h>
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Manager/Manager_Base.h"

enum class EBlendState : uint8_t
{
	Opaque,
	AlphaBlend,
	Additive,
	NonPremultiplied
};

enum class EDepthStencilState : uint8_t
{
	DepthNone,
	DepthDefault,
	DepthRead,
	DepthReverseZ,
	DepthReadReverseZ
};

enum class ERasterState : uint8_t
{
	CullNone,
	CW,
	CCW,
	WireFrame
};

enum class ESamplerState : uint8_t
{
	PointWrap,
	PointClamp,
	LinearWrap,
	LinearClamp,
	AnisotropicWrap,
	AnisotropicClamp
};


// 이 클래스 매크로 추천
#define Renderer XD3DDevice::Get()

class XD3DDevice final : public TSingleton<XD3DDevice>
{
private:
	void Initialize_Internal();

public:
	void Release();

public:
	void ClearColor(const struct FLinearColor& InColor) const;
	void Present();

public:
	void SetPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY InTopology) const;
	void SetBlendState(const EBlendState InState) const;
	void SetDepthStencilState(const EDepthStencilState InState) const;
	void SetSamplerState(const ESamplerState InState, int32_t* InSlots, int32_t InSize) const;
	void SetRasterState(const ERasterState InState) const;

public:
#pragma region Get
	FORCEINLINE ID3D11Device*           GetDevice() const { return mDevice.Get(); }
	FORCEINLINE ID3D11DeviceContext*    GetImmediateDeviceContext() const { return mImmediateContext.Get(); }
	FORCEINLINE IDXGISwapChain1*        GetSwapChain() const { return mSwapChain.Get(); }
	FORCEINLINE ID2D1Factory*           Get2DFactory() const { return mD2DFactory.Get(); }
	FORCEINLINE IDWriteFactory*         GetWriteFactory() const { return mDWriteFactory.Get(); }
	FORCEINLINE ID2D1SolidColorBrush*   GetBrush() const { return mColorBrush.Get(); }
	FORCEINLINE ID3D11RenderTargetView* GetRTV() const { return mRenderTargetView.Get(); }
	FORCEINLINE ID2D1RenderTarget*      GetSurface1() const { return mRenderTarget_2D.Get(); }
	FORCEINLINE CommonStates*           GetDXTKCommonStates() const { return mToolKitStates.get(); }
	FORCEINLINE ID3D11RasterizerState*  GetRasterizerState() const { return mRasterizerState.Get(); }


	FORCEINLINE static CHAR* GetVideoCardDesc() { return Get().mVideoCardDescription; }
#pragma endregion

private:
	void CreateDevice();
	void CreateGIFactory();
	void CreateSwapChain();
	void Create2DResources();
	void CreateRasterizerState();
	void SetRenderTarget();
	void SetViewportSize(uint32_t InWidth, uint32_t InHeight);
	void ResizeSwapChain(uint32_t width, uint32_t height);
	void CleanResources();

	void OnResize(uint32_t InWidth, uint32_t InHeight);

private:
	ComPtr<ID3D11Device>           mDevice;						/** 디바이스 포인터 (리소스 생성) */
	ComPtr<ID3D11DeviceContext>    mImmediateContext;           /** 디바이스 컨텍스트 포인터 (파이프라인 설정) */
	ComPtr<ID3D11DeviceContext>    mDeferredContext_1;          /** 지연 컨텍스트 포인터 (멀티 스레드 렌더링) */
	ComPtr<IDXGIFactory2>          mGIFactory;					/** Graphic Infrastructure*/
	ComPtr<ID2D1Factory>           mD2DFactory;					/** D2D Factory*/
	ComPtr<IDWriteFactory>         mDWriteFactory;				/** DWrite Factory*/
	ComPtr<ID2D1SolidColorBrush>   mColorBrush;					/** Brush */
	ComPtr<IDXGISwapChain1>        mSwapChain;                  /** 스왑체인 포인터 (디스플레이 제어) */
	ComPtr<ID2D1RenderTarget>      mRenderTarget_2D;			/** DWrite Draw RTV */
	ComPtr<ID3D11RenderTargetView> mRenderTargetView;           /** 화면에 보여지는 버퍼 개체 (RTV) */
	ComPtr<ID3D11RasterizerState>  mRasterizerState;			/** 래스터라이저 상태 관리 개체 인터페이스 */

	UPtr<CommonStates> mToolKitStates;

	DXGI_SWAP_CHAIN_DESC1 mSwapChainDesc;             /** 스왑체인 구조체 */
	D3D11_VIEWPORT        mViewport;                  /** 렌더링 뷰포트 */
	CHAR                  mVideoCardDescription[128]; /** 비디오카드 상세 정보 */

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<XD3DDevice>;
	friend class MManagerInterface;

	XD3DDevice();
	~XD3DDevice();

public:
	XD3DDevice(const XD3DDevice&)            = delete;
	XD3DDevice& operator=(const XD3DDevice&) = delete;

#pragma endregion
};

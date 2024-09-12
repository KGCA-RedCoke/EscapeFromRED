#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Manager/Manager_Base.h"
#include <directxtk/CommonStates.h>
#include "Core/Interface/ICoreInterface.h"

using namespace Microsoft::WRL;

#define G_Context GraphicDevice::Get()

class GraphicDevice final : public ICoreInterface, public TSingleton<GraphicDevice>
{
public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Release() override;
#pragma endregion

	void ClearColor(const struct FLinearColor& InColor) const;
	void Present();

public:
	void SetDepthEnable(bool bEnable) const;

public:
#pragma region Get
	FORCEINLINE static ID3D11Device*           GetDevice() { return Get().mDevice.Get(); }
	FORCEINLINE static ID3D11DeviceContext*    GetImmediateDeviceContext() { return Get().mImmediateContext.Get(); }
	FORCEINLINE static IDXGISwapChain1*        GetSwapChain() { return Get().mSwapChain.Get(); }
	FORCEINLINE static ID2D1Factory*           Get2DFactory() { return Get().mD2DFactory.Get(); }
	FORCEINLINE static IDWriteFactory*         GetWriteFactory() { return Get().mDWriteFactory.Get(); }
	FORCEINLINE static ID2D1SolidColorBrush*   GetBrush() { return Get().mColorBrush.Get(); }
	FORCEINLINE static ID3D11RenderTargetView* GetRTV() { return Get().mRenderTargetView.Get(); }
	FORCEINLINE static ID2D1RenderTarget*      GetSurface1() { return Get().mRenderTarget_2D.Get(); }
	FORCEINLINE static ID3D11DepthStencilView* GetDepthStencilView() { return Get().mDepthStencilView.Get(); }
	FORCEINLINE static CommonStates*           GetDXTKCommonStates() { return Get().mCommonStates.get(); }


	FORCEINLINE static CHAR* GetVideoCardDesc() { return Get().mVideoCardDescription; }
#pragma endregion

private:
	void CreateDevice();
	void CreateGIFactory();
	void CreateSwapChain();
	void Create2DResources();
	void SetDepthStencil();
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
	ComPtr<ID3D11DepthStencilView> mDepthStencilView;           /** 깊이/스텐실 정보 기반 뷰 관리 개체 */

	// 따로 빼자..
	ComPtr<ID3D11DepthStencilState> mDepthStencilState;			/**	 */
	ComPtr<ID3D11Texture2D>         mDepthStencilBuffer;        /** 2D 이미지 관리 개체 인터페이스 */

	std::unique_ptr<CommonStates> mCommonStates;

	DXGI_SWAP_CHAIN_DESC1 mSwapChainDesc;             /** 스왑체인 구조체 */
	D3D11_VIEWPORT        mViewport;                  /** 렌더링 뷰포트 */
	CHAR                  mVideoCardDescription[128]; /** 비디오카드 상세 정보 */

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<GraphicDevice>;

	GraphicDevice();
	~GraphicDevice() override;

public:
	GraphicDevice(const GraphicDevice&)            = delete;
	GraphicDevice& operator=(const GraphicDevice&) = delete;

#pragma endregion
};

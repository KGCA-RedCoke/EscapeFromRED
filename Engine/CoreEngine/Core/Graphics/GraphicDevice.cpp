#include "common_pch.h"
#include "GraphicDevice.h"

#include "Core/Utils/Logger.h"
#include "Debug/Assert.h"
#include "Core/Utils/Math/Color.h"
#include "Viewport/MViewportManager.h"
#include "Core/Window/Window.h"

GraphicDevice::GraphicDevice()
	: mSwapChainDesc(),
	  mViewport(),
	  mVideoCardDescription{} {}

GraphicDevice::~GraphicDevice()
{
	GraphicDevice::Release();
}

void GraphicDevice::Initialize()
{
	if (!XMVerifyCPUSupport())
	{
		MessageBox(nullptr,
				   TEXT("This application requires the processor support SSE2 instructions."),
				   TEXT("CPU not Support"),
				   MB_OK | MB_ICONEXCLAMATION);
		assert(-1);
	}

	CreateGIFactory();
	CreateDevice();
	CreateSwapChain();

	Create2DResources();

	OnResize(Window::GetWindow()->GetWindowWidth(), Window::GetWindow()->GetWindowHeight());


	Window::GetWindow()->ResizeCallbacks.emplace_back([this](UINT InWidth, UINT InHeight){
		OnResize(InWidth, InHeight);
	});

}

void GraphicDevice::Update(float_t DeltaTime)
{
	mImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	SetDepthEnable(true);

	mImmediateContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
	mImmediateContext->RSSetViewports(1, &mViewport);
}

void GraphicDevice::Release()
{
	
	mCommonStates       = nullptr;
	mDevice             = nullptr;
	mImmediateContext   = nullptr;
	mDeferredContext_1  = nullptr;
	mGIFactory          = nullptr;
	mD2DFactory         = nullptr;
	mDWriteFactory      = nullptr;
	mSwapChain          = nullptr;
	mRenderTargetView   = nullptr;
	mDepthStencilView   = nullptr;
	mDepthStencilState  = nullptr;
	mDepthStencilBuffer = nullptr;
	mColorBrush         = nullptr;

}

void GraphicDevice::ClearColor(const FLinearColor& InColor) const
{
	mImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), InColor.RGBA);
}

void GraphicDevice::Present()
{
	// 후면 버퍼 렌더
	CheckResult(
				mSwapChain->Present(
									Window::GetWindow()->IsVsyncEnabled(),
									0
								   ));
}

void GraphicDevice::SetDepthEnable(bool bEnable) const
{
	mImmediateContext->OMSetDepthStencilState(bEnable ? mDepthStencilState.Get() : nullptr, 1);
}

void GraphicDevice::CreateDevice()
{
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	D3D_FEATURE_LEVEL outFeatureLevel;

	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) | defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG; // 디버그 활성화
#endif

	CheckResult(
				D3D11CreateDevice(
								  nullptr,                  // 주 모니터 사용
								  D3D_DRIVER_TYPE_HARDWARE, // 하드웨어 가속 사용
								  nullptr,                  // 하드웨어 사용
								  flags,					// flags
								  featureLevels,                    // 기능 수준
								  ARRAYSIZE(featureLevels),         // 기능 배열 개수
								  D3D11_SDK_VERSION,                // DX Version
								  mDevice.GetAddressOf(),           // (Out) Device
								  &outFeatureLevel,                 // (Out) Features
								  mImmediateContext.GetAddressOf() // (Out) DeviceContext
								 ));
	mCommonStates = std::make_unique<CommonStates>(G_Context.GetDevice());

	CheckResult(mDevice->CreateDeferredContext(FALSE, mDeferredContext_1.GetAddressOf()));

	if (outFeatureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		mImmediateContext = nullptr;
		mDevice           = nullptr;
	}
}

void GraphicDevice::CreateGIFactory()
{
	ComPtr<IDXGIAdapter> DXGIAdapter;

	CheckResult(
				CreateDXGIFactory(
								  __uuidof(IDXGIFactory),
								  reinterpret_cast<void**>(mGIFactory.GetAddressOf()
								  )));

	CheckResult(mGIFactory->EnumAdapters(0, DXGIAdapter.GetAddressOf()));

	DXGI_ADAPTER_DESC desc;
	CheckResult(DXGIAdapter->GetDesc(&desc));

	size_t stringLength;
	wcstombs_s(&stringLength, mVideoCardDescription, 128, desc.Description, 128);

	LOG_CORE_INFO("그래픽카드: {}, 메모리: {:d}", mVideoCardDescription, desc.DedicatedVideoMemory / (1 << 20));

	DXGIAdapter = nullptr;
}

/*
 * blt-model(버퍼 복사) -> flip-model (버퍼 교체)
 * SwapChainDesc -> SwapChainDesc1
 */
void GraphicDevice::CreateSwapChain()
{
	ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	{

		mSwapChainDesc.BufferCount        = 2;
		mSwapChainDesc.Width              = Window::GetWindow()->GetWindowWidth(); // Buffer Width
		mSwapChainDesc.Height             = Window::GetWindow()->GetWindowHeight(); // Buffer Height
		mSwapChainDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM; // 색상 출력 형식
		mSwapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 버퍼 (렌더링 버퍼)
		mSwapChainDesc.SampleDesc.Count   = 1; // 멀티 샘플링 개수
		mSwapChainDesc.SampleDesc.Quality = 0; // 멀티 샘플링 품질
		mSwapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Swap이 일어난 이후 버퍼를 Discard
		mSwapChainDesc.Scaling            = DXGI_SCALING_NONE; // Scaling 없음
		mSwapChainDesc.Stereo             = FALSE; // 스테레오 사용하지 않음
		mSwapChainDesc.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	}

	CheckResult(
				mGIFactory->CreateSwapChainForHwnd(
												   mDevice.Get(),
												   Window::GetWindow()->GetWindowHandle(),
												   &mSwapChainDesc,
												   nullptr,
												   nullptr,
												   reinterpret_cast<IDXGISwapChain1**>(mSwapChain.GetAddressOf())
												  ));
}

void GraphicDevice::Create2DResources()
{
	CheckResult(
				D2D1CreateFactory(
								  D2D1_FACTORY_TYPE_SINGLE_THREADED, // 멀티 스레드 지원 가능
								  mD2DFactory.GetAddressOf()
								 ));

	CheckResult(
				DWriteCreateFactory(
									DWRITE_FACTORY_TYPE_SHARED, // SHARED | ISOLATED타입 (메모리)
									__uuidof(IDWriteFactory),
									reinterpret_cast<IUnknown**>(mDWriteFactory.GetAddressOf())
								   ));
}

void GraphicDevice::SetDepthStencil()
{
	// Depth Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width              = mSwapChainDesc.Width;
	depthStencilDesc.Height             = mSwapChainDesc.Height;
	depthStencilDesc.MipLevels          = 1;
	depthStencilDesc.ArraySize          = 1;
	depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags     = 0;
	depthStencilDesc.MiscFlags          = 0;
	CheckResult(
				mDevice->CreateTexture2D(
										 &depthStencilDesc,
										 nullptr,
										 mDepthStencilBuffer.GetAddressOf()
										));

	// Depth Stencil View
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	CheckResult(
				mDevice->CreateDepthStencilView(
												mDepthStencilBuffer.Get(),
												&depthStencilViewDesc,
												mDepthStencilView.GetAddressOf()
											   ));

	// Depth Stencil State
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};
	depthStencilStateDesc.DepthEnable    = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
	CheckResult(
				mDevice->CreateDepthStencilState(
												 &depthStencilStateDesc,
												 mDepthStencilState.GetAddressOf()
												));
}

void GraphicDevice::SetRenderTarget()
{
	// 3D Side RenderTarget
	ComPtr<ID3D11Texture2D> backBuffer;
	ComPtr<IDXGISurface>    dxgiBackBuffer;

	CheckResult(
				mSwapChain->GetBuffer(
									  0, // 백버퍼 인덱스 (front, back버퍼 두개 뿐이므로) 0
									  __uuidof(ID3D11Texture2D),
									  reinterpret_cast<LPVOID*>(backBuffer.GetAddressOf())
									 ));
	CheckResult(
				mDevice->
				CreateRenderTargetView( // RenderTargetView가 생성된 이후로는 직접 BackBuffer에 접근하지 말고 View를 이용한다. (backBuffer해제)
									   backBuffer.Get(), // BackBuffer로 부터 View를 생성
									   nullptr, // 일반적인 경우 Default값 NULL을 넣어줘도 무방
									   mRenderTargetView.GetAddressOf()
									  ));
	mImmediateContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	// 2D Side RenderTarget (DWrite) 
	CheckResult(
				backBuffer->QueryInterface( // Query Interface
										   __uuidof(IDXGISurface), // 내가 원하는 자료형(ID)
										   reinterpret_cast<void**>(dxgiBackBuffer.GetAddressOf() // 담아질 개체
										   )));

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
				mD2DFactory->CreateDxgiSurfaceRenderTarget(
														   dxgiBackBuffer.Get(),
														   &props,
														   mRenderTarget_2D.GetAddressOf()
														  ));

	CheckResult(
				mRenderTarget_2D->CreateSolidColorBrush(
														D2D1::ColorF(D2D1::ColorF::White),
														mColorBrush.GetAddressOf()
													   ));

	dxgiBackBuffer = nullptr;
	backBuffer     = nullptr;
}

void GraphicDevice::SetViewportSize(uint32_t InWidth, uint32_t InHeight)
{
	mViewport.Width    = static_cast<float_t>(InWidth);
	mViewport.Height   = static_cast<float_t>(InHeight);
	mViewport.MinDepth = 0.f;
	mViewport.MaxDepth = 1.f;
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mImmediateContext->RSSetViewports(1, &mViewport);
}

void GraphicDevice::ResizeSwapChain(uint32_t width, uint32_t height)
{
	CheckResult(
				mSwapChain->ResizeBuffers(
										  mSwapChainDesc.BufferCount,
										  width,
										  height,
										  mSwapChainDesc.Format,
										  mSwapChainDesc.Flags)
			   );
	CheckResult(mSwapChain->GetDesc1(&mSwapChainDesc));
}

void GraphicDevice::CleanResources()
{
	mDepthStencilState  = nullptr;
	mDepthStencilView   = nullptr;
	mDepthStencilBuffer = nullptr;

	mImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);

	mColorBrush       = nullptr;
	mRenderTargetView = nullptr;
	mRenderTarget_2D  = nullptr;
}

void GraphicDevice::OnResize(uint32_t InWidth, uint32_t InHeight)
{
	CleanResources();

	ResizeSwapChain(InWidth, InHeight);

	SetDepthStencil();
	SetRenderTarget();
	SetViewportSize(InWidth, InWidth);
}

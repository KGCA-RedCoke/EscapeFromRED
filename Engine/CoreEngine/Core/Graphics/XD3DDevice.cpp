#include "XD3DDevice.h"

#include "Core/Interface/IRenderable.h"
#include "Core/Utils/Logger.h"
#include "Core/Utils/Graphics/DXUtils.h"
#include "Core/Utils/Math/Color.h"
#include "Core/Window/Window.h"
#include "Debug/Assert.h"

XD3DDevice::XD3DDevice()
	: mSwapChainDesc(),
	  mViewport(),
	  mVideoCardDescription{}
{
	Initialize_Internal();
}

XD3DDevice::~XD3DDevice()
{
	Release();
}

void XD3DDevice::Initialize_Internal()
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
	CreateRasterizerState();
	Create2DResources();

	OnResize(Window::GetWindow()->GetWindowWidth(), Window::GetWindow()->GetWindowHeight());


	Window::GetWindow()->ResizeCallbacks.emplace_back([this](UINT InWidth, UINT InHeight){
		OnResize(InWidth, InHeight);
	});
}


void XD3DDevice::Release()
{
	mToolKitStates     = nullptr;
	mDevice            = nullptr;
	mImmediateContext  = nullptr;
	mDeferredContext_1 = nullptr;
	mGIFactory         = nullptr;
	mD2DFactory        = nullptr;
	mDWriteFactory     = nullptr;
	mSwapChain         = nullptr;
	mRenderTargetView  = nullptr;
	mRasterizerState   = nullptr;
	mColorBrush        = nullptr;
}

void XD3DDevice::ClearColor(const FLinearColor& InColor) const
{
	mImmediateContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);
	mImmediateContext->RSSetViewports(1, &mViewport);

	// mImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), InColor.RGBA);
}

void XD3DDevice::ClearCommandList()
{
	mRenderCommandList.clear();
}

void XD3DDevice::QueueCommand(const uint32_t InGeometryType, const FRenderCommand& InCommandList)
{
	mRenderCommandList[InGeometryType].push_back(InCommandList);
}

void XD3DDevice::Draw()
{
	for (auto& [key, value] : mRenderCommandList)
	{
		for (auto& command : value)
		{
			command.ObjectToRender->Draw();
		}
		mImmediateContext->DrawIndexedInstanced(
												0,
												value.size(),
												0,
												0,
												0);
	}

	// 후면 버퍼 렌더
	CheckResult(
				mSwapChain->Present(
									Window::GetWindow()->IsVsyncEnabled(),
									0
								   ));

	ClearCommandList();
}

void XD3DDevice::SetPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY InTopology) const
{
	mImmediateContext->IASetPrimitiveTopology(InTopology);
}

void XD3DDevice::SetBlendState(EBlendState InState) const
{
	ID3D11BlendState* newBlendState = nullptr;

	switch (InState)
	{
	case EBlendState::Opaque:
		newBlendState = mToolKitStates->Opaque();
		break;
	case EBlendState::AlphaBlend:
		newBlendState = mToolKitStates->AlphaBlend();
		break;
	case EBlendState::Additive:
		newBlendState = mToolKitStates->Additive();
		break;
	case EBlendState::NonPremultiplied:
		newBlendState = mToolKitStates->NonPremultiplied();
		break;
	}

	mImmediateContext->OMSetBlendState(newBlendState, nullptr, 0xFFFFFFFF);
}

void XD3DDevice::SetDepthStencilState(const EDepthStencilState InState) const
{
	ID3D11DepthStencilState* newDepthStencilState = nullptr;

	switch (InState)
	{
	case EDepthStencilState::DepthNone:
		newDepthStencilState = mToolKitStates->DepthNone();
		break;
	case EDepthStencilState::DepthDefault:
		newDepthStencilState = mToolKitStates->DepthDefault();
		break;
	case EDepthStencilState::DepthRead:
		newDepthStencilState = mToolKitStates->DepthRead();
		break;
	case EDepthStencilState::DepthReverseZ:
		newDepthStencilState = mToolKitStates->DepthReverseZ();
		break;
	case EDepthStencilState::DepthReadReverseZ:
		newDepthStencilState = mToolKitStates->DepthReadReverseZ();
		break;
	}

	mImmediateContext->OMSetDepthStencilState(newDepthStencilState, 0);
}

void XD3DDevice::SetSamplerState(const ESamplerState InState, int32_t* InSlots, int32_t InSize) const
{
	ID3D11SamplerState* newSamplerState = nullptr;
	switch (InState)
	{
	case ESamplerState::PointWrap:
		newSamplerState = mToolKitStates->PointWrap();
		break;
	case ESamplerState::PointClamp:
		newSamplerState = mToolKitStates->PointClamp();
		break;
	case ESamplerState::LinearWrap:
		newSamplerState = mToolKitStates->LinearWrap();
		break;
	case ESamplerState::LinearClamp:
		newSamplerState = mToolKitStates->LinearClamp();
		break;
	case ESamplerState::AnisotropicWrap:
		newSamplerState = mToolKitStates->AnisotropicWrap();
		break;
	case ESamplerState::AnisotropicClamp:
		newSamplerState = mToolKitStates->AnisotropicClamp();
		break;
	}

	for (int32_t i = 0; i < InSize; ++i)
	{
		int32_t slot = InSlots[i];
		mImmediateContext->PSSetSamplers(slot, 1, &newSamplerState);
	}


}

void XD3DDevice::SetRasterState(const ERasterState InState) const
{
	ID3D11RasterizerState* newRasterState = nullptr;

	switch (InState)
	{
	case ERasterState::CullNone:
		newRasterState = mToolKitStates->CullNone();
		break;
	case ERasterState::CW:
		newRasterState = mToolKitStates->CullClockwise();
		break;
	case ERasterState::CCW:
		newRasterState = mToolKitStates->CullCounterClockwise();
		break;
	case ERasterState::WireFrame:
		newRasterState = mToolKitStates->Wireframe();
		break;
	}

	assert(newRasterState);
	mImmediateContext->RSSetState(newRasterState);
}

void XD3DDevice::CreateDevice()
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
								  mDevice.GetAddressOf(),           // (Out) RenderManager
								  &outFeatureLevel,                 // (Out) Features
								  mImmediateContext.GetAddressOf() // (Out) DeviceContext
								 ));
	mToolKitStates = std::make_unique<CommonStates>(mDevice.Get());

	CheckResult(mDevice->CreateDeferredContext(FALSE, mDeferredContext_1.GetAddressOf()));

	if (outFeatureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		mImmediateContext = nullptr;
		mDevice           = nullptr;
	}
}

void XD3DDevice::CreateGIFactory()
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

	size_t  stringLength;
	errno_t err = wcstombs_s(&stringLength, mVideoCardDescription, 128, desc.Description, 128);
	if (err != 0)
	{
		LOG_CORE_ERROR("wcstombs_s failed");
	}

	LOG_CORE_INFO("그래픽카드: {}, 메모리: {:d}", mVideoCardDescription, desc.DedicatedVideoMemory / (1 << 20));

	DXGIAdapter = nullptr;
}

/*
 * blt-model(버퍼 복사) -> flip-model (버퍼 교체)
 * SwapChainDesc -> SwapChainDesc1
 * flip-model을 사용하면 멀티 샘플링 (MSAA)를 사용할 수 없다.
 * FXAA를 사용하거나 다른 방법을 사용해야 한다.
 */
void XD3DDevice::CreateSwapChain()
{
	ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	{
		mSwapChainDesc.BufferCount        = 2;
		mSwapChainDesc.Width              = Window::GetWindow()->GetWindowWidth(); // Buffer Width
		mSwapChainDesc.Height             = Window::GetWindow()->GetWindowHeight(); // Buffer Height
		mSwapChainDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM; // 색상 출력 형식
		mSwapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 버퍼 (렌더링 버퍼)
		mSwapChainDesc.SampleDesc.Count   = 1; // 멀티 샘플링 개수 (고정)
		mSwapChainDesc.SampleDesc.Quality = 0; // 멀티 샘플링 품질 (고정)
		mSwapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Swap이 일어난 이후 버퍼를 Discard
		mSwapChainDesc.Scaling            = DXGI_SCALING_NONE; // Scaling 없음
		mSwapChainDesc.Stereo             = FALSE; // 스테레오 사용하지 않음
		mSwapChainDesc.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
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

void XD3DDevice::Create2DResources()
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

void XD3DDevice::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	{
		rasterizerDesc.FillMode              = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode              = D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthBias             = D3D11_DEFAULT_DEPTH_BIAS;
		rasterizerDesc.DepthBiasClamp        = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizerDesc.SlopeScaledDepthBias  = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizerDesc.DepthClipEnable       = true;
		rasterizerDesc.ScissorEnable         = false;
		rasterizerDesc.MultisampleEnable     = false;
		rasterizerDesc.AntialiasedLineEnable = true;
	}

	CheckResult(
				mDevice->CreateRasterizerState(
											   &rasterizerDesc,
											   mRasterizerState.GetAddressOf()
											  ));
}

void XD3DDevice::SetRenderTarget()
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
	// mImmediateContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

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

void XD3DDevice::SetViewportSize(uint32_t InWidth, uint32_t InHeight)
{
	Utils::DX::CreateViewport(InWidth, InHeight, &mViewport);
	mImmediateContext->RSSetViewports(1, &mViewport);
}

void XD3DDevice::ResizeSwapChain(uint32_t width, uint32_t height)
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

void XD3DDevice::CleanResources()
{
	mImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);

	mColorBrush       = nullptr;
	mRenderTargetView = nullptr;
	mRenderTarget_2D  = nullptr;
}

void XD3DDevice::OnResize(uint32_t InWidth, uint32_t InHeight)
{
	CleanResources();

	ResizeSwapChain(InWidth, InHeight);

	SetRenderTarget();
	SetViewportSize(InWidth, InHeight);
}

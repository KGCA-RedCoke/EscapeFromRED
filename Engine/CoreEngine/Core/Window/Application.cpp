#include "Application.h"

#include "Core/Entity/Actor/JActor.h"
#include "Core/Entity/Actor/JStaticMeshActor.h"
#include "Core/Entity/Camera/JCamera.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Font/JFont.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Timer.h"
#include "Core/Utils/Math/Color.h"
#include "Core/Utils/ObjectLoader/FbxFile.h"
#include "Core/Window/Window.h"

CBuffer::Light g_LightData = {FVector4(1, 2, -1, 0), FVector4::OneVector};


Application* Application::s_AppInstance = nullptr;

/**
 * 기본생성자의 경우 Renderer라는 이름의 창을 생성하고 1600x900의 크기로 생성한다.
 */
Application::Application()
	: Application(L"Renderer", FBasicWindowData(1600, 900, false, false)) {}

/**
 * 커스텀 생성자의 경우 사용자가 지정한 이름과 크기로 창을 생성한다.
 * @param WindowTitle 어플리케이션 이름
 * @param WindowData 창의 기본 데이터 
 */
Application::Application(LPCWSTR WindowTitle, const FBasicWindowData& WindowData)
{
	if (s_AppInstance != nullptr)
	{
		LOG_CORE_ERROR("Application is already created.");
		delete this;
		assert(false);
		return;
	}

	s_AppInstance = this;
	mWindow       = std::make_unique<Window>(WindowTitle, WindowData);
}

/** 모든 개체들을 스마트포인터로 관리하기에 앱 종료시에 처리, 구현할 내용은 없다. */
Application::~Application()
{
	Release();
};

void Application::Initialize()
{
	if (bRunning)
	{
		LOG_CORE_ERROR("Application is already running.");
		return;
	}

	ResetValues();

	//---------------------------------- 초기화 --------------------------------------------
	mWindow->Initialize();	// 가장 먼저 윈도우 창을 초기화한다.

	IManager.Initialize();	// 통합 매니저 인터페이스를 통해 초기화한다. (자세한 파이프라인은 MManagerInterface.cpp 참조)

	// 프레임 표시용 텍스트 FIXME: 이것도 매니저(Object)로 관리해야함
	auto viewportPtr = IManager.ViewportManager->FetchResource(Name_Editor_Viewport);
	assert(viewportPtr);
	viewportPtr->OnViewportResized.Bind([&](uint32_t InWidth, uint32_t InHeight){
		mFpsText->SetRenderTarget(IManager.ViewportManager->FetchResource(Name_Editor_Viewport)->RTV_2D.Get());
	});
	mFpsText = MakeUPtr<JFont>();
	mFpsText->Initialize();
	mFpsText->SetFontSize(36);
	mFpsText->SetColor(FLinearColor::Gallary);
	mFpsText->SetScreenPosition({5, 5});

	Actors.reserve(10);

	Ptr<JActor> sampleActor = MakePtr<JStaticMeshActor>("Preview Actor",
														IManager.MeshManager->CreateOrClone(Path_Mesh_Sphere));
	sampleActor->Initialize();
	sampleActor->SetLocalScale({500, 500, 500});


	Actors.push_back(sampleActor);
}

void Application::Run()
{
	// 개체 초기화
	Initialize();

	while (bRunning)
	{
		mCurrentTime = mTimer->Elapsed();

		// 매 프레임 작동
		HandleFrame();

		// 1초마다 작동
		if (mTimer->Elapsed() >= mTime + 1.f)
		{
			// 매 초마다 작동
			HandleTick();
		}

		// 윈도우 X 버튼을 누르거나 프로그램 종료 확인
		CheckWindowClosure();
	}

}

void Application::Update(float DeltaTime)
{
	IManager.Update(DeltaTime);

	mFpsText->Update(DeltaTime);
	mFpsText->SetText(std::format(L"fps: {:d}", mFramesPerSec));
}

void Application::Render()
{
	IManager.RenderManager->ClearColor(FLinearColor::TrueBlack);

	IManager.Render(); // GUI Render


	for (int32_t i = 0; i < Actors.size(); ++i)
	{
		Actors[i]->Tick(mDeltaTime);
		Actors[i]->Draw(IManager.RenderManager->GetImmediateDeviceContext());
	}

	mFpsText->Draw(IManager.RenderManager->GetImmediateDeviceContext());

	IManager.RenderManager->Draw();
}

void Application::Release()
{
	mFpsText = nullptr;

	IManager.Release();
}

void Application::HandleFrame()
{
	Timer frameTimer;
	frameTimer.Reset();
	{
		Update(mDeltaTime);

		Render();
	}
	mFrameCounter++;
	mDeltaTime = frameTimer.Elapsed();
}

void Application::HandleTick()
{
	mTime += 1.f;

	mFramesPerSec = mFrameCounter;

	mFrameCounter = 0;
	// TODO: Tick
}


void Application::CheckWindowClosure()
{
	mWindow->Update();

	if (mWindow->IsClosed())
	{
		bRunning = false;
	}
}

void Application::ResetValues()
{
	assert(mTimer == nullptr); // 뭔가 잘못됐다...

	mTimer     = std::make_unique<Timer>();
	bRunning   = true;
	bMinimized = false;
	mTime      = 0.f;
}

uint32_t Application::GetWindowWidth() const
{
	return mWindow->GetWindowWidth();
}

uint32_t Application::GetWindowHeight() const
{
	return mWindow->GetWindowHeight();
}

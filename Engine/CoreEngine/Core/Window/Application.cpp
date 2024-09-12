#include "common_pch.h"
#include "Application.h"
#include "Core/Entity/Camera/JCamera.h"
#include "Core/Graphics/Font/JFont.h"
#include "Core/Graphics/Shader/JDXObject.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Timer.h"
#include "Core/Utils/Math/Color.h"
#include "Core/Utils/ObjectLoader/FbxFile.h"
#include "Core/Window/Window.h"

FVector4 g_DirectionalLightPos   = {500, 1200, -1000, 1};
FVector4 g_DirectionalLightColor = {1.f, 0.976f, 0.992f, 1}; // 6500k 주광색 (완벽히 같진 않음)


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
Application::~Application() = default;

void Application::Run()
{
	// 개체 초기화
	Initialize();

	while (bRunning)
	{
		mCurrentTime = mTimer->ElapsedMillis();

		// 매 프레임 작동
		HandleFrame();

		// 1초마다 작동
		if (mTimer->Elapsed() - mTime > 1.f)
		{
			// 매 초마다 작동
			HandleTick();
		}

		// 윈도우 X 버튼을 누르거나 프로그램 종료 확인
		CheckWindowClosure();
	}

	// 관리 개체 해제
	Application::Release();
}

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
	mFpsText = std::make_unique<JFont>(IManager.ViewportManager.FetchResource(Name_Editor_Viewport)->RTV_2D.Get());
	mFpsText->Initialize();
	mFpsText->SetFontSize(48);
	mFpsText->SetColor(FLinearColor::Orange);
	mFpsText->SetScreenPosition({25, 25});

	// // FIXME: Test Code
	Utils::Fbx::FbxFile g_testObj("Game/Model/Male_Jacket.fbx");
	g_testObj.Load();
	mDXObject = std::make_unique<JDXObject>(&g_testObj);
}

void Application::Update(float DeltaTime)
{
	IManager.Update(DeltaTime);

	mFpsText->Update(DeltaTime);
	mFpsText->SetText(std::format(L"fps: {:d}", mFramesPerSec));
}

void Application::Render()
{
	G_Context.ClearColor(FLinearColor::EbonyClay);

	IManager.Render(); // GUI Render

	mDXObject->PreRender();
	mDXObject->PostRender();

	mFpsText->PreRender();
	mFpsText->Render();
	mFpsText->PostRender();

	G_Context.Present();
}

void Application::Release()
{
	mFpsText = nullptr;

	IManager.Release();
}

uint32_t Application::GetWindowWidth()
{
	return mWindow->GetWindowWidth();
}

uint32_t Application::GetWindowHeight()
{
	return mWindow->GetWindowHeight();
}

void Application::HandleFrame()
{
	Timer frameTimer;
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
	mTimer     = std::make_unique<Timer>();
	bRunning   = true;
	bMinimized = false;
	mTime      = 0.f;
}

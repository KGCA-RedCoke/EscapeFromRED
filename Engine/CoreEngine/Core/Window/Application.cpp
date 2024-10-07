#include "Application.h"

#include "Core/Entity/Actor/JActor.h"
#include "Core/Entity/Camera/JCamera.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Font/JFont.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Timer.h"
#include "Core/Utils/Math/Color.h"
#include "Core/Utils/ObjectLoader/FbxFile.h"
#include "Core/Window/Window.h"

FVector4 g_DirectionalLightPos   = {500, 1200, 1000, 1};
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
	mFpsText = std::make_unique<JFont>(IManager.ViewportManager->FetchResource(Name_Editor_Viewport)->RTV_2D.Get());
	mFpsText->Initialize();
	mFpsText->SetFontSize(48);
	mFpsText->SetColor(FLinearColor::Orange);
	mFpsText->SetScreenPosition({25, 25});

	// FIXME: Test Code
	// Utils::Fbx::FbxFile fbxLoader;
	// fbxLoader.Load("rsc/Engine/Mesh/Primitive/Sphere.fbx");
	// fbxLoader.Load("rsc/Engine/Mesh/Primitive/Cube.fbx");
	// fbxLoader.Load("rsc/Engine/Mesh/Primitive/Cylinder.fbx");
	// fbxLoader.Load("rsc/Engine/Mesh/Primitive/Cone.fbx");
	// fbxLoader.Load("rsc/Engine/Mesh/Primitive/Plane.fbx");

	mActors.reserve(10);
	for (int32_t i = 0; i < 10; ++i)
	{
		// mRenderObjects.push_back(MakeUPtr<JMeshObject>());
	}
	Ptr<JMeshObject>          swordMesh      = IManager.MeshManager->CreateOrLoad("Game/Mesh/CyberPunk_A.jasset");
	Ptr<JStaticMeshComponent> swordComponent = MakePtr<JStaticMeshComponent>("Sword");
	Ptr<JActor>               sampleActor    = MakePtr<JActor>("SampleActor");
	sampleActor->Initialize();
	// swordComponent->SetMeshObject(swordMesh);
	// sampleActor->AttachSceneComponent(swordComponent);
	mActors.push_back(sampleActor);
	//
	// sampleActor->SetLocalLocation({10, 0, 0});

	// Utils::Serialization::DeSerialize("Game/Mesh/Cube.jasset", mRenderObjects[0].get());
	// Utils::Serialization::DeSerialize("Game/Mesh/Sphere.jasset", mRenderObjects[1].get());
	// Utils::Serialization::DeSerialize("Game/Mesh/Cone.jasset", mRenderObjects[2].get());
	// Utils::Serialization::DeSerialize("Game/Mesh/Cylinder.jasset", mRenderObjects[3].get());
	// Utils::Serialization::DeSerialize("Game/Mesh/Plane.jasset", mRenderObjects[4].get());
	// Utils::Serialization::DeSerialize("Game/Mesh/SM_CP_Mid_Male_Body.jasset", mRenderObjects[5].get());


	// // Utils::Serialization::DeSerialize("Game/Bot.jasset", mRenderObjects[5].get());
	// Utils::Serialization::DeSerialize("Game/CyberPunk_A.jasset", mRenderObjects[6].get());
	// // Utils::Serialization::DeSerialize("Game/King.jasset", mRenderObjects[7].get());
	// Utils::Serialization::DeSerialize("Game/Male_Jacket.jasset", mRenderObjects[8].get());
	// Utils::Serialization::DeSerialize("Game/axis.jasset", mRenderObjects[9].get());


	// Utils::Serialization::DeSerialize_Implement("Game/Cube.jasset", mDXObject.get());
	// Utils::Serialization::DeSerialize_Implement("rsc/Cylinder.jasset", mDXObject2.get());

	// size_t objHalfSize = mRenderObjects.size() / 2;
	// for (int32_t i = 0; i < mRenderObjects.size(); ++i)
	// {
	// 	mRenderObjects[i]->SetTranslation({/*-5 * objHalfSize +*/ i * 5.f, 0, 0});
	// }

}

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

}

void Application::Update(float DeltaTime)
{
	IManager.Update(DeltaTime);

	mFpsText->Update(DeltaTime);
	mFpsText->SetText(std::format(L"fps: {:d}", mFramesPerSec));
}

void Application::Render()
{
	IManager.RenderManager->ClearColor(FLinearColor::EbonyClay);

	IManager.Render(); // GUI Render

	for (int32_t i = 0; i < mActors.size(); ++i)
	{
		mActors[i]->Tick(mDeltaTime);
		mActors[i]->Draw();
	}

	mFpsText->PreRender();
	mFpsText->Render();
	mFpsText->PostRender();

	IManager.RenderManager->Present();
}

void Application::Release()
{
	mFpsText = nullptr;

	IManager.Release();
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

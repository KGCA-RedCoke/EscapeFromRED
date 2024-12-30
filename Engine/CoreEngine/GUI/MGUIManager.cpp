﻿#include "MGUIManager.h"
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include "GUI_AssetBrowser.h"
#include "GUI_Inspector.h"
#include "GUI_Themes.h"
#include "Camera/GUI_Camera_Debug_Frustum.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/ObjectLoader/FbxFile.h"
#include "Core/Window/Window.h"
#include "Editor/GUI_Editor_Material.h"
#include "Editor/LandScape/GUI_Editor_LandScape.h"
#include "Editor/UI/GUI_Editor_UI.h"
#include "Popup/GUI_Popup_FileBrowser.h"
#include "Utils/GUI_Settings_Collision.h"
#include "Viewport/GUI_Viewport_Scene.h"

constexpr char Name_Viewport[]     = "Editor Viewport";
constexpr char Name_Inspector[]    = "Editor Inspector";
constexpr char Name_AssetBrowser[] = "Asset Browser";

MGUIManager::MGUIManager()
{};

MGUIManager::~MGUIManager() = default;

/**
 * ImGui는 기본적으로 렌더링을 위한 초기화가 필요.
 * ImGui 초기화는 ImGui 컨텍스트를 생성하고, 스타일을 설정하며, 플랫폼 및 백엔드를 초기화해야한다.
 */
void MGUIManager::Initialize(ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();	// ImGui 컨텍스트 생성

	// 아래는 ImGui 설정과 관련된 변수(전역 설정)
	ImGuiIO&    io    = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// 키보드 컨트롤 활성화
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Docking(다른 창에 붙이기) 활성화
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// 뷰포트 활성화 

	ImGui::Spectrum::StyleColorsSpectrum();	// 스펙트럼 테마 적용(테마는 io로 다양하게 설정이 가능하다)
	ImGui::Spectrum::LoadFont(18);

	/// NOTE:만약 특정 창에 대해서만 스타일을 설정하고 싶다면 (Push이후엔 Pop으로 설정을 되돌릴 수 있다, Pop을 안하면 계속 적용된다...)
	/// ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	/// ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	/// ImGui::PopStyleColor();
	/// ImGui::PopStyleVar();
	// if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	// {
	// 	style.WindowRounding              = 0.0f;
	// 	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	// }

	// 플랫폼, 백엔드 초기화
	ImGui_ImplWin32_Init(Window::GetWindow()->GetWindowHandle());
	ImGui_ImplDX11_Init(InDevice, InDeviceContext);

	InitializeStaticGUI();

	ScaleAllSize(1.5f);
}

void MGUIManager::InitializeStaticGUI()
{
	mStaticGUI[0] = Load<GUI_AssetBrowser>(Name_AssetBrowser);
	mStaticGUI[1] = Load<GUI_Viewport_Scene>(Name_Viewport);
	mStaticGUI[2] = Load<GUI_Inspector>(Name_Inspector);

	mStaticGUI[0]->Initialize();
	mStaticGUI[1]->Initialize();
	mStaticGUI[2]->Initialize();
}

void MGUIManager::Release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void MGUIManager::UpdateMainMenuBar()
{
	if (GetWorld.bGameMode)
		return;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8("파일")))
		{
			if (ImGui::MenuItem(u8("새 레벨 생성")))
			{
				// New Scene
			}
			if (ImGui::MenuItem(u8("레벨 열기")))
			{
				// Open Scene
			}
			if (ImGui::MenuItem(u8("현재 레벨 저장")))
			{
				JLevel* activeLevel = MLevelManager::Get().GetActiveLevel();
				Utils::Serialization::Serialize(activeLevel->GetPath().c_str(), activeLevel);
			}

			if (ImGui::MenuItem(u8("FBX")))
			{
				bOpenFileBrowser = true;
			}
			if (ImGui::MenuItem(u8("에셋 브라우저")))
			{
				static int32_t count = 0;
				Load<GUI_AssetBrowser>(std::format("{}_{}", Name_AssetBrowser, count++))->Initialize();
			}
			if (ImGui::MenuItem(u8("종료")))
			{
				PostQuitMessage(0);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8("에디터")))
		{
			if (ImGui::MenuItem(u8("콜리전 매니저")))
			{
				// Collision Manager
				if (GUI_Settings_Collision* collisionManager = Load<GUI_Settings_Collision>("Collision Manager"))
				{
					collisionManager->bIsWindowOpen = true;
				}
			}

			if (ImGui::MenuItem(u8("머티리얼 에디터")))
			{
				if (GUI_Editor_Material* materialEditor = Load<GUI_Editor_Material>("Material Editor"))
				{
					mMaterialEditorRef = materialEditor;
				}

				mMaterialEditorRef->bIsWindowOpen = true;

			}

			if (ImGui::MenuItem(u8("UI 에디터")))
			{
				if (GUI_Editor_UI* uiEditor = Load<GUI_Editor_UI>("UI Editor"))
				{
					mUIEditorRef = uiEditor;
				}
				mUIEditorRef->bIsWindowOpen = true;
			}

			if (ImGui::MenuItem(u8("지형 에디터")))
			{
				// LandScape Editor
				if (!mLandScapeEditorRef)
				{
					if (GUI_Editor_LandScape* landScapeEditor = Load<GUI_Editor_LandScape>("LandScape Editor"))
					{
						mLandScapeEditorRef = landScapeEditor;
					}
				}

				mLandScapeEditorRef->bIsWindowOpen = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8("렌더 상태 뷰")))
		{
			if (ImGui::MenuItem(u8("와이어 프레임")))
			{
				bRenderWireFrame = true;
			}
			if (ImGui::MenuItem(u8("머티리얼")))
			{
				bRenderWireFrame = false;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8("카메라")))
		{
			if (ImGui::MenuItem("Frustum View"))
			{
				if (auto ptr = Load<GUI_Camera_Debug_Frustum>("Debug Frustum"))
				{
					ptr->bIsWindowOpen = true;
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8("에디터 테마")))
		{}

		ImGui::EndMainMenuBar();  // Make sure to close the main menu bar
	}
}


void MGUIManager::UpdateGUIs(float DeltaTime) const
{
	if (GetWorld.bGameMode)
	{
		mStaticGUI[1]->Update(DeltaTime);
		return;
	}
	for (auto it = mManagedList.begin(); it != mManagedList.end(); ++it)
	{
		if (const auto& ptr = it->second)
		{
			ptr->Update(DeltaTime);
		}
	}
}

void MGUIManager::Update(float_t DeltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();

	UpdateMainMenuBar();

	UpdateGUIs(DeltaTime);

	if (bOpenFileBrowser)
	{
		JText path = "rsc/";
		JText resultFile;
		if (ImGui::OpenFileBrowser(path, ImGui::FileBrowserOption::FILE, {".fbx"}))
		{
			if (ImGui::FetchFileBrowserResult(path, resultFile))
			{
				if (std::filesystem::exists(resultFile) && std::filesystem::is_regular_file(resultFile))
				{
					// Import Fbx
					Utils::Fbx::FbxFile fbxFile;
					if (!fbxFile.Load(resultFile.c_str()))
					{
						LOG_CORE_INFO("Fbx File Failed: {}", resultFile);
					}
				}
				bOpenFileBrowser = false;
			}
		}
	}

}

/**
 * 여러 GUI창들의 Update(Frame)에서 생성된 렌더링 명령들을 GPU에 전달 (한번에)
 * 그래서 업데이트와 렌더링 로직은 Update에 있지만 실제 렌더링은 Render에서 이루어진다.
 */
void MGUIManager::Render()
{
	ImGui::Render(); // ImGui 렌더링 명령 생성
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // 생성된 명령(GetDrawData)을 GPU에 전달

	// Update and Present additional Platform Windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows(); // 새 뷰포트 생성 | 기존 뷰포트 상태, 위치 업데이트 | 필요없는 뷰포트 파괴 등
		ImGui::RenderPlatformWindowsDefault(); // 각 뷰포트 렌더
	}

	if (GetWorld.bGameMode)
	{
		mStaticGUI[1]->Render();
		return;
	}
	// for (auto it = mManagedList.begin(); it != mManagedList.end(); ++it)
	// {
	// 	if (const auto& ptr = it->second)
	// 	{
	// 		if (ptr->bIsWindowOpen)
	// 		{
	// 			ptr->Render();
	// 		}
	// 	}
	// }
}

void MGUIManager::ScaleAllSize(float InScale)
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(InScale);
}

GUI_Inspector* MGUIManager::GetInspector() const
{
	if (!mStaticGUI[2])
	{
		return nullptr;
	}
	return dynamic_cast<GUI_Inspector*>(mStaticGUI[2]);
}

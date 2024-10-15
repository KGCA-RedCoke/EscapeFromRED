#include "MGUIManager.h"
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include "GUI_AssetBrowser.h"
#include "GUI_Inspector.h"
#include "GUI_Themes.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Utils/ObjectLoader/FbxFile.h"
#include "Core/Window/Window.h"
#include "Editor/GUI_Editor_Material.h"
#include "Editor/LandScape/GUI_Editor_LandScape.h"
#include "Popup/GUI_Popup_FileBrowser.h"
#include "Viewport/GUI_Viewport_Scene.h"

constexpr char Name_Viewport[]     = "Editor Viewport";
constexpr char Name_Inspector[]    = "Editor Inspector";
constexpr char Name_AssetBrowser[] = "Asset Browser";

MGUIManager::MGUIManager()
{
	Initialize_Initialize();
};
MGUIManager::~MGUIManager() = default;

/**
 * ImGui는 기본적으로 렌더링을 위한 초기화가 필요.
 * ImGui 초기화는 ImGui 컨텍스트를 생성하고, 스타일을 설정하며, 플랫폼 및 백엔드를 초기화해야한다.
 */
void MGUIManager::Initialize_Initialize()
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

	/// NOTE:만약 특정 창에 대해서만 스타일을 설정하고 싶다면 (Push이후엔 Pop으로 설정을 되돌릴 수 있다, Pop을 안하면 계속 적용된다...)
	/// ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	/// ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	/// ImGui::PopStyleColor();
	/// ImGui::PopStyleVar();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding              = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// 플랫폼, 백엔드 초기화
	ImGui_ImplWin32_Init(Window::GetWindow()->GetWindowHandle());
	ImGui_ImplDX11_Init(IManager.RenderManager->GetDevice(), IManager.RenderManager->GetImmediateDeviceContext());

	InitializeStaticGUI();
	// ImGui::GetIO().FontGlobalScale = 1.5f; // 기본 글자 크기보다 1.5배로 확대
}

void MGUIManager::InitializeStaticGUI()
{
	mStaticGUI[0] = CreateOrLoad<GUI_AssetBrowser>(Name_AssetBrowser);
	mStaticGUI[1] = CreateOrLoad<GUI_Viewport_Scene>(Name_Viewport);
	mStaticGUI[2] = CreateOrLoad<GUI_Inspector>(Name_Inspector);

	mStaticGUI[0]->Initialize();
	mStaticGUI[1]->Initialize();
	mStaticGUI[2]->Initialize();
}

void MGUIManager::UpdateMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				// New Scene
			}
			if (ImGui::MenuItem("Open Scene"))
			{
				// Open Scene
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				// Save Scene
			}
			if (ImGui::MenuItem("Exit"))
			{
				// Exit
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Utils"))
		{
			if (ImGui::MenuItem("Material Editor"))
			{
				JText title = std::format("Material Editor {}", GUI_Editor_Material::GetAvailableIndex());
				if (Ptr<GUI_Editor_Material> materialEditor = CreateOrLoad<GUI_Editor_Material>(title))
				{
					mMaterialEditorList.push_back(materialEditor);
				}
			}

			if (ImGui::MenuItem("LandScape Editor"))
			{
				// LandScape Editor
				if (Ptr<GUI_Editor_LandScape> landScapeEditor = CreateOrLoad<GUI_Editor_LandScape>("LandScape Editor"))
				{
					mLandScapeEditor = landScapeEditor;
				}
			}

			if (ImGui::MenuItem("Fbx Importer"))
			{
				bOpenFileBrowser = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("RenderState"))
		{
			if (ImGui::MenuItem("WireFrame"))
			{
				bRenderWireFrame = true;
			}
			if (ImGui::MenuItem("Material"))
			{
				bRenderWireFrame = false;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();  // Make sure to close the main menu bar
	}
}

void MGUIManager::UpdateStaticGUI(float DeltaTime) const
{
	for (int32_t i = 0; i < 3; ++i)
	{
		mStaticGUI[i]->Update(DeltaTime);
	}
}

void MGUIManager::UpdateEditorGUI(float DeltaTime) const
{
	// Material Editor
	for (int32_t i = 0; i < mMaterialEditorList.size(); ++i)
	{
		mMaterialEditorList[i]->Update(DeltaTime);
	}

	if (mLandScapeEditor)
	{
		mLandScapeEditor->Update(DeltaTime);
	}
}

void MGUIManager::Update(float_t DeltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();

	UpdateMainMenuBar();

	UpdateStaticGUI(DeltaTime);

	UpdateEditorGUI(DeltaTime);

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
					if (fbxFile.Load(resultFile.c_str()))
					{
						LOG_CORE_INFO("Fbx File Loaded: {}", resultFile);
					}
				}
				bOpenFileBrowser = false;
			}
		}
	}

}

void MGUIManager::Release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
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

	for (auto it = mManagedList.begin(); it != mManagedList.end(); ++it)
	{
		it->second->Render();
	}
}

void MGUIManager::AddGUI(EGUIType InType)
{}

void MGUIManager::HideGUI(EGUIType InType)
{}

void MGUIManager::DeleteGUI(EGUIType InType)
{}

void MGUIManager::ScaleAllSize(float InScale)
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(InScale);
}

Ptr<GUI_Inspector> MGUIManager::GetInspector() const
{
	return std::dynamic_pointer_cast<GUI_Inspector>(mStaticGUI[2]);
}

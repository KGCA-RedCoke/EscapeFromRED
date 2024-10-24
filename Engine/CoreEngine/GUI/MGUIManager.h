#pragma once
#include "GUI_Base.h"
#include "Core/Manager/Manager_Base.h"

class GUI_Editor_LandScape;
class GUI_Inspector;
class GUI_Editor_Material;

enum class EGUIType : uint8_t
{
	Viewport = 0,
	Inspector,
	AssetBrowser,
	Scene,
	Console,
	Settings,
	Max
};

/**
 * GUI Manager
 * 모든 GUI 개체 관리 및 렌더
 */
class MGUIManager : public Manager_Base<GUI_Base, MGUIManager>
{
private:
	void Initialize_Initialize();

public:
	void Update(float_t DeltaTime);
	void Release();

	void Render();

public:
	void AddGUI(EGUIType InType);
	void HideGUI(EGUIType InType);
	void DeleteGUI(EGUIType InType);
	void ScaleAllSize(float InScale);

	Ptr<GUI_Inspector> GetInspector() const;

	bool IsRenderWireFrame() const { return bRenderWireFrame; }

private:
	/**
	 * 엔진 필수 GUI를 생성하고 초기화
	 */
	void InitializeStaticGUI();

	void UpdateMainMenuBar();

	/**
	 * 엔진 필수 GUI들을 업데이트
	 */
	void UpdateStaticGUI(float DeltaTime) const;

	void UpdateEditorGUI(float DeltaTime) const;

private:
	Ptr<GUI_Editor_Material>  mMaterialEditorRef;
	Ptr<GUI_Editor_LandScape> mLandScapeEditorRef;
	Ptr<GUI_Base>             mStaticGUI[static_cast<int32_t>(EGUIType::Max)];
	bool                      bOpenFileBrowser;
	bool                      bRenderWireFrame = false;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MGUIManager>;
	friend class MManagerInterface;

	MGUIManager();
	~MGUIManager();

public:
	MGUIManager(const MGUIManager&)            = delete;
	MGUIManager& operator=(const MGUIManager&) = delete;

#pragma endregion
};

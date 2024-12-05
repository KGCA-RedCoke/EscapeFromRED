#pragma once
#include "GUI_Base.h"
#include "Core/Manager/Manager_Base.h"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

class GUI_Editor_UI;
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
	void Initialize(class ID3D11Device* InDevice, class ID3D11DeviceContext* InDeviceContext);

public:
	void Release();

	void Update(float_t DeltaTime);
	void Render();

public:
	void ScaleAllSize(float InScale);

	GUI_Inspector* GetInspector() const;

	bool IsRenderWireFrame() const { return bRenderWireFrame; }

private:
	/**
	 * 엔진 필수 GUI를 생성하고 초기화
	 */
	void InitializeStaticGUI();

	void UpdateMainMenuBar();

	void UpdateGUIs(float DeltaTime) const;

private:
	GUI_Editor_Material*  mMaterialEditorRef;
	GUI_Editor_UI*        mUIEditorRef;
	GUI_Editor_LandScape* mLandScapeEditorRef;
	GUI_Base*             mStaticGUI[static_cast<int32_t>(EGUIType::Max)];
	bool                  bOpenFileBrowser;
	bool                  bRenderWireFrame = false;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MGUIManager>;
	friend class JWorld;

	MGUIManager();
	~MGUIManager();

public:
	MGUIManager(const MGUIManager&)            = delete;
	MGUIManager& operator=(const MGUIManager&) = delete;

#pragma endregion
};

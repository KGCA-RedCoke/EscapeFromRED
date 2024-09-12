#pragma once
#include "Core/Interface/ICoreInterface.h"
#include "Core/Manager/Manager_Base.h"
#include "imgui/GUI_Base.h"

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
class MGUIManager : public ICoreInterface, public Manager_Base<GUI_Base, MGUIManager>
{
public:
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Release() override;

	void Render();

public:
	void AddGUI(EGUIType InType);
	void HideGUI(EGUIType InType);
	void DeleteGUI(EGUIType InType);
	void ScaleAllSize(float InScale);

private:
	/**
	 * 엔진 필수 GUI를 생성하고 초기화
	 */
	void InitializeStaticGUI();

	/**
	 * 엔진 필수 GUI들을 업데이트
	 */
	void UpdateStaticGUI(float DeltaTime);


#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MGUIManager>;
	friend class MManagerInterface;

	MGUIManager();
	~MGUIManager() override;

public:
	MGUIManager(const MGUIManager&)            = delete;
	MGUIManager& operator=(const MGUIManager&) = delete;

#pragma endregion
};

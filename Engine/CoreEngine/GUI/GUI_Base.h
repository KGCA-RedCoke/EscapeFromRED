#pragma once
#include <imgui/imgui.h>
#include "common_include.h"

class GUI_Base
{
public:
	GUI_Base(const JText& InTitle);
	virtual ~GUI_Base() = default;

public:
	virtual void Initialize() {}
	virtual void Update(float DeltaTime);
	virtual void Release() {}
	virtual void Render() {}

public:
	[[nodiscard]] bool             IsWindowDocked() const;
	FORCEINLINE [[nodiscard]] bool IsWindowOpen() const { return bIsWindowOpen; }
	void                           OpenIfNotOpened();

protected:
	/** 창 메뉴바 구성은 여기서 */
	virtual void ShowMenuBar() {}

	/** 내부 구현은 모두 여기서 */
	virtual void Update_Implementation(float DeltaTime) {}

	/** 이 윈도우가 docking이 안되어 있으면 스타일을 변경한다. */
	virtual void ChangeWindowStyleIfNotDocked();

protected:
	std::string      mTitle;
	bool             bIsWindowOpen;
	bool             bIsVisible;
	ImGuiWindowFlags mWindowFlags = 0;

	friend class MGUIManager;
};

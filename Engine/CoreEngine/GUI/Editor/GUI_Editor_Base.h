#pragma once
#include "GUI/GUI_Base.h"

class GUI_Editor_Base : public GUI_Base
{
public:
	GUI_Editor_Base(const JText& InTitle);
	~GUI_Editor_Base() override = default;

	void Render() override;

public:
	void OpenIfNotOpened();

protected:
	void Update_Implementation(float DeltaTime) override;
	void ShowMenuBar() override;

protected:
	Ptr<class JCamera>       mCamera;	// 뷰포트 카메라
	Ptr<class FViewportData> mViewport;	// 뷰포트
	float                    mDeltaTime;

	ImVec2 mWindowSize;

};

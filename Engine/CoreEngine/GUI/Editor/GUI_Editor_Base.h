#pragma once
#include "GUI/GUI_Base.h"

class GUI_Editor_Base : public GUI_Base
{
public:
	GUI_Editor_Base(const JText& InTitle);
	~GUI_Editor_Base() override = default;

public:
	void Render() override;

protected:
	void Update_Implementation(float DeltaTime) override;
	void ShowMenuBar() override;

protected:
	class JCamera*       mCamera;	// 뷰포트 카메라
	class FViewportData* mViewport;	// 뷰포트
	float                mDeltaTime;

	ImVec2 mWindowSize;

};

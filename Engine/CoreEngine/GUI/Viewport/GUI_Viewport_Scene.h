#pragma once

#include "GUI_Viewport.h"

class JTexture;

class GUI_Viewport_Scene : public GUI_Viewport
{
public:
	explicit GUI_Viewport_Scene(const JText& InTitle);
	~GUI_Viewport_Scene() override = default;

public:
	void Initialize() override;

private:
	void Update_Implementation(float DeltaTime) override;

	void ShowTopMenu();

private:
	JTexture* mPauseIcon;
	JTexture* mPlayIcon;
};

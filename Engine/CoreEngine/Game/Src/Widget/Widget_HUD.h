#pragma once
#include "Core/Entity/UI/MUIManager.h"

class Widget_HUD : public JWidgetComponent
{
public:
	Widget_HUD() = default;
	Widget_HUD(const JText& InName);
	~Widget_HUD() override = default;

public:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

private:
	// Timer (0, 0.85)
	JUIComponent* mGameTimerText;

	// 
	JUIComponent* mCookieCountText;
	JUIComponent* mColaCountText;
};

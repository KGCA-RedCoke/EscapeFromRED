#pragma once
#include "Core/Entity/UI/MUIManager.h"

class Widget_IntroScene : public JWidgetComponent
{
public:
	Widget_IntroScene() = default;
	Widget_IntroScene(const JText& InName);
	~Widget_IntroScene() override = default;

public:
	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

private:
	JUIComponent* mPressAnyKey;
};

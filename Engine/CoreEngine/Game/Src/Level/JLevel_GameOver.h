#pragma once
#include "Core/Entity/Level/JLevel.h"

class JAudioComponent;

class JLevel_GameOver : public JLevel
{
public:
	JLevel_GameOver();
	~JLevel_GameOver() override;

public:
	void InitializeLevel() override;
	void UpdateLevel(float DeltaTime) override;
	void RenderLevel() override;

private:
	FVector2          mStartButtonSize;
	uint32_t          mButtonIndex = 0;
	JWidgetComponent* mGameOverWidget;
	JAudioComponent*  mIntroSound;
	JAudioComponent*  mButtonSound;
};

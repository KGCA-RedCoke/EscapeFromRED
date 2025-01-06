#pragma once
#include "Core/Entity/Level/JLevel.h"

class JAudioComponent;
class JUIComponent;

class JLevel_Intro : public JLevel
{
public:
	JLevel_Intro();
	~JLevel_Intro() override;

public:
	void InitializeLevel() override;
	void UpdateLevel(float DeltaTime) override;
	void RenderLevel() override;

private:
	FVector2 mStartButtonSize;
	JAudioComponent* mIntroSound;
	JAudioComponent* mButtonSound;
};

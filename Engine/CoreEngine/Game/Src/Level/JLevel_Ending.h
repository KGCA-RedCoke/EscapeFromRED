#pragma once
#include "Core/Entity/Level/JLevel.h"

class JAudioComponent;

class JLevel_Ending : public JLevel
{
public:
	JLevel_Ending();
	~JLevel_Ending() override;

public:
	void InitializeLevel() override;
	void UpdateLevel(float DeltaTime) override;
	void RenderLevel() override;

private:
	JWidgetComponent* mEndingWidget;
	JAudioComponent*  mIntroSound;
	JAudioComponent*  mButtonSound;
};

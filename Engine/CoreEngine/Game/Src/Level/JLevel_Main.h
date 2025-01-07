#pragma once
#include "Core/Entity/Level/JLevel.h"


class JAudioComponent;

class JLevel_Main : public JLevel
{
public:
	JLevel_Main();
	~JLevel_Main() override;

public:
	void InitializeLevel() override;
	void UpdateLevel(float DeltaTime) override;
	void RenderLevel() override;

private:
	JAudioComponent* mMainSound;
};

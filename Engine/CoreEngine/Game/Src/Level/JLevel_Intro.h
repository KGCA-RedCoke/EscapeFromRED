#pragma once
#include "Core/Entity/Level/JLevel.h"

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

public:
	uint32_t      GetUIComponentCount() const { return mUIComponents.size(); }
	JUIComponent* GetUIComponent(uint32_t Index) const;

private:
	JArray<JUIComponent*> mUIComponents;
};

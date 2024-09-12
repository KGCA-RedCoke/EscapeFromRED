#pragma once
#include <map>
#include "common_include.h"
#include "GUI_Base.h"

class GUI_Inspector : public GUI_Base
{
public:
	GUI_Inspector(const std::string& InTitle);
	~GUI_Inspector() override = default;

public:
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Release() override;


private:
	std::map<JText, class JSceneComponent*> mSceneComponents;
	float                                   RotationYaw = 0;
};

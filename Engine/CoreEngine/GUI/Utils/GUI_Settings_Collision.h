#pragma once
#include "GUI/GUI_Base.h"

class GUI_Settings_Collision : public GUI_Base
{
public:
	GUI_Settings_Collision(const JText& InTitle);
	~GUI_Settings_Collision() override = default ;

public:
	void Initialize() override;

protected:
	void Update_Implementation(float DeltaTime) override;

};

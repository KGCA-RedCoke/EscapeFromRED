#pragma once
#include "Core/Entity/LandScape/JLandScape.h"
#include "GUI/GUI_Base.h"

class GUI_Editor_LandScape : public GUI_Base
{
public:
	GUI_Editor_LandScape(const JText& InTitle);
	~GUI_Editor_LandScape() override;

public:
	void Render() override;

private:
	void Update_Implementation(float DeltaTime) override;

public:
	void GenerateLandScape();

private:
	FMapDesc mLandScapeDesc;
};

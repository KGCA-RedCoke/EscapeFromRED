#pragma once
#include "GUI/Editor/GUI_Editor_Base.h"

class GUI_Editor_UI : public GUI_Editor_Base
{
public:
	GUI_Editor_UI(const JText& InTitle);
	~GUI_Editor_UI() override = default;

public:
	void Render() override;
};

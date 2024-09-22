#pragma once
#include <imgui/imgui.h>
#include "common_include.h"
#include "Core/Interface/ICoreInterface.h"

class GUI_Base : public ICoreInterface
{
public:
	explicit GUI_Base(const JText& InTitle);
	~GUI_Base() override = default;

public:
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Release() override;

public:
	bool IsWindowDocked() const;

protected:
	/** 이 윈도우가 docking이 안되어 있으면 스타일을 변경한다. */
	virtual void ChangeWindowStyleIfNotDocked();

protected:
	std::string mTitle;
	bool        bIsWindowOpen;
};

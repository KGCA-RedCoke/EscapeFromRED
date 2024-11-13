#pragma once

#include "common_include.h"
#include "GUI_Base.h"
#include "Utils/GUI_Utils.h"

class GUI_Inspector : public GUI_Base
{
public:
	GUI_Inspector(const std::string& InTitle);
	~GUI_Inspector() override = default;

	void AddSceneComponent(const JText& InName, const Ptr<class JActor>& InSceneComponent);

	JHash<JText, WPtr<class JActor>> GetSelectedSceneComponent() const { return mSceneComponents; }
	void SetSelectedActor(const Ptr<class JSceneComponent>& Ptr) { mSelectedSceneComponent = Ptr; }

private:
	void Update_Implementation(float DeltaTime) override;

	void DrawSearchBar();
	void DrawTreeNode(const Ptr<class JSceneComponent>& InSceneComponent);
	void DrawDetails();

private:
	Utils::GUI::FSearchBar           mSearchBar;
	Ptr<JSceneComponent>             mSelectedSceneComponent;
	JHash<JText, WPtr<class JActor>> mSceneComponents;
};

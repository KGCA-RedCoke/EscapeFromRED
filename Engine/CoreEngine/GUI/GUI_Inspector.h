#pragma once

#include "common_include.h"
#include "GUI_Base.h"

class GUI_Inspector : public GUI_Base
{
public:
	GUI_Inspector(const std::string& InTitle);
	~GUI_Inspector() override = default;

	void AddSceneComponent(const JText& InName, const Ptr<class JSceneComponent>& InSceneComponent);

	JHash<JText, WPtr<class JSceneComponent>> GetSelectedSceneComponent() const { return mSceneComponents; }

private:
	void Update_Implementation(float DeltaTime) override;

	void DrawSearchBar();
	void DrawTreeNode(const Ptr<class JSceneComponent>& InSceneComponent);

private:
	ImGuiTextFilter                           mFilter;
	Ptr<JSceneComponent>                      mSelectedSceneComponent;
	JHash<JText, WPtr<class JSceneComponent>> mSceneComponents;
};

#pragma once

#include "common_include.h"
#include "GUI_Base.h"
#include "Utils/GUI_Utils.h"

class JLevel;

class GUI_Inspector : public GUI_Base
{
public:
	GUI_Inspector(const std::string& InTitle);
	~GUI_Inspector() override = default;

	void AddSceneComponent(const JText& InName, class AActor* InSceneComponent);

	JHash<JText, AActor*> GetSelectedSceneComponent() const { return mSceneComponents; }
	void                  SetSelectedActor(class JSceneComponent* Ptr) { mSelectedSceneComponent = Ptr; }

private:
	void Update_Implementation(float DeltaTime) override;

	void DrawSearchBar();
	void DrawTreeNode(JSceneComponent* InSceneComponent);

	void DrawDetails();
	void DrawName();
	void DrawTransform() const;
	void DrawFlags();
	void DrawMaterialSlot();

private:
	Utils::GUI::FSearchBar mSearchBar;
	JSceneComponent*       mSelectedSceneComponent;
	JLevel*                mLevel;
	JHash<JText, AActor*>  mSceneComponents;

	bool bRequestDelete = false;
	bool bRequestCopy   = false;
};

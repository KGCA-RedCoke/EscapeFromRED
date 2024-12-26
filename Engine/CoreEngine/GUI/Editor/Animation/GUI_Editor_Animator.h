#pragma once
#include "GUI/GUI_Editor_Node.h"

class JAnimator;

class GUI_Editor_Animator : public GUI_Editor_Node
{
public:
	GUI_Editor_Animator(const JText& );
	~GUI_Editor_Animator() override = default;

public:
	void Initialize() override;
	void Render() override;
	void Release() override;

protected:
	void Update_Implementation(float DeltaTime) override;
	void ShowMenuBar() override;

private:
	JAnimator* mAnimator = nullptr;
	ed::FNode* SpawnAnimSequenceNode(const JText& InState, const JText& InAnimClipText);
};

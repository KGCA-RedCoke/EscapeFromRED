#pragma once

#include "GUI_Base.h"
#include "GUI_Node.h"
#include "imgui/imgui_node_editor_internal.h"

namespace ed = ax::NodeEditor;


class GUI_Editor_Node : public GUI_Base
{
public:
	GUI_Editor_Node(const JText& InTitle);
	~GUI_Editor_Node() override = default;

public:
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Update_Implementation(float DeltaTime) override;
	void Release() override;

protected:
	void UpdateTouch(float DeltaTime);

	void BuildNode(ed::FNode* InNode);

protected:
	ed::EditorContext* mContext = nullptr;

	JArray<UPtr<ed::FNode>> mNodes;
	JHash<uint32_t, float>  mNodeTouchTime;

	JText mSaveFilePath;
};

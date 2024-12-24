#pragma once
#include <imgui/imgui_node_editor.h>

#include "common_include.h"


namespace ax::NodeEditor
{
	enum class PinType
	{
		Flow,
		Bool,
		Int,
		Float,
		String,
		Object,
		Function,
		Delegate,
	};

	enum class NodeType
	{
		Blueprint,
		Simple,
		Tree,
		Comment,
		AnimSequence,
	};

	struct LinkInfo
	{
		LinkId Id;
		PinId  InputId;
		PinId  OutputId;
	};

	/**
	 * 노드 연결 핀(입력, 출력)
	 */
	struct FPin
	{
		PinId         ID;
		JText         Name;
		PinType       Type;
		PinKind       Kind;
		struct FNode* Node;

		FPin(uint32_t InID, JText InName, PinType InType)
			: ID(InID),
			  Name(std::move(InName)),
			  Type(InType),
			  Kind(PinKind::Input),
			  Node(nullptr)
		{}
	};

	/**
 * 노드 연결 링크
 */
	struct FLink
	{
		LinkId  ID;
		PinId   StartPin;
		PinId   EndPin;
		ImColor Color = ImColor(255, 255, 255, 255);

		FLink(LinkId InID, PinId InStartPin, PinId InEndPin);
	};

	/**
	 * 노드 데이터
	 */
	struct FNode
	{
		uint32_t ID;
		NodeId   NodeID;
		JText    Title = "Empty";
		JText    Name;
		ImColor  Color = ImColor(255, 255, 255, 255);
		NodeType Type  = NodeType::Simple;
		ImVec2   Size  = ImVec2(0, 0);
		ImVec2   Pos   = ImVec2(0, 0);

		JArray<FPin> Inputs;
		JArray<FPin> Outputs;

		JText           State;
		JText           SavedState;
		ImVector<FLink> Links;

		bool bIsEditing = false;

	public:
		FNode(JText InName);
		virtual      ~FNode() = default;
		void         Update();
		virtual void Update_Impl();
	};


	// bool CanCreateLink(FPin* a, FPin* b)
	// {
	// 	if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
	// 		return false;
	//
	// 	return true;
	// }
}

#pragma once
#include <stack>
#include "Node.h"

class BT_TEST;

// namespace Bt
// {
//     class Node;
// }

class BTBuilder
{
public:
	BTBuilder();
	~BTBuilder();

	Ptr<Bt::Node>             RootNode;
	std::stack<Ptr<Bt::Node>> NodeStack;

public:
	template <typename T>
	BTBuilder&    CreateRoot();
	BTBuilder&    AddSequence(const JText& Name);
	BTBuilder&    AddSelector(const JText& Name);
	BTBuilder&    AddDecorator(std::function<NodeStatus()> Condition);
	BTBuilder&    AddActionNode(std::function<NodeStatus()> Action);
	BTBuilder&    EndBranch();
	Ptr<Bt::Node> Build();
};

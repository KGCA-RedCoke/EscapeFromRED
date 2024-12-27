#pragma once
#include "common_include.h"

class BT_BOSS;

enum class NodeStatus
{
    Success,
    Failure,
    Running
};

namespace Bt
{
    class Node
    {
    public:
        virtual ~Node();
        virtual NodeStatus tick() = 0;

        void AddChild(Ptr<Node> child) { children.push_back(child); }

    protected:
        std::vector<Ptr<Bt::Node>> children;

    public:
        int runningNodeIdx = 0;
    };
}


class ActionNode : public Bt::Node
{
public:
    ActionNode(std::function<NodeStatus()> action);

    NodeStatus tick() override;

private:
    std::function<NodeStatus()> action;
};

class Selector : public Bt::Node
{
public:
    JText NodeName = "";

public:
    Selector();
    Selector(const JText& name);

    NodeStatus tick() override;
};

class Sequence : public Bt::Node
{
public:
    JText NodeName = "";

public:
    Sequence();
    Sequence(const JText& name);

    NodeStatus tick() override;
};

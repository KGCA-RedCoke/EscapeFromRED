#include "Node.h"
#include "BtBase.h"

Bt::Node::~Node()
{}

ActionNode::ActionNode(std::function<NodeStatus()> action): action(action)
{}

NodeStatus ActionNode::tick()
{
    return action();
}

Selector::Selector()
{
}

Selector::Selector(const JText& name): NodeName(name)
{}

NodeStatus Selector::tick()
{
    for (int i = runningNodeIdx; i < children.size(); i++)
    {
        NodeStatus status = children[i]->tick();
        if (status == NodeStatus::Running)
            runningNodeIdx = i;
        else
            runningNodeIdx = 0;
        if (status != NodeStatus::Failure)
            return status;
    }
    return NodeStatus::Failure;
}

Sequence::Sequence()
{
}

Sequence::Sequence(const JText& name): NodeName(name)
{}

NodeStatus Sequence::tick()
{
    for (int i = runningNodeIdx; i < children.size(); i++)
    {
        NodeStatus status = children[i]->tick();
        if (status == NodeStatus::Running)
            runningNodeIdx = i;
        else
            runningNodeIdx = 0;
        if (status != NodeStatus::Success)
            return status;
    }
    return NodeStatus::Success;
}

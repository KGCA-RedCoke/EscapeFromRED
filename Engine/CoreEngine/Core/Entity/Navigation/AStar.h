#pragma once
#include <queue>
#include "common_include.h"
#include "Node.h"
#include "Core/Entity/Component/JActorComponent.h"
#include "Core/Input/XKeyboardMouse.h"

namespace NAV
{
    class Node;
}

struct CompareNode
{
    bool operator()(const std::shared_ptr<NAV::Node>& a, const std::shared_ptr<NAV::Node>& b) const
    {
        int aFCost = a->FCost();
        int bFCost = b->FCost();
        return aFCost > bFCost || (aFCost == bFCost && a->HCost > b->HCost);
    }
};

using PriorityQueue = std::priority_queue<Ptr<NAV::Node>, std::vector<Ptr<NAV::Node>>, CompareNode>;
using UnOrdSet = std::unordered_set<Ptr<NAV::Node>>;

class AStar : public JActorComponent
{
public:
    AStar(): JActorComponent()
    {
        mInputKeyboard.Initialize();
    }

    AStar(JTextView InName) : JActorComponent(InName) { mInputKeyboard.Initialize(); }

    void Initialize() override;
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    void Destroy() override;

    void FindPath(Ptr<NAV::Node> Start, Ptr<NAV::Node> Target);
    int GetDistance(Ptr<NAV::Node> A, Ptr<NAV::Node> B);
    void RetracePath(Ptr<NAV::Node> Start, Ptr<NAV::Node> Target);
    void FollowPath(float DeltaTime);

    float mSpeed = 1000;
    float mRotateSpeed = 300;
    bool IsPosUpdated = false;
    bool PushToggle = false;
    bool PushHold = false;
    FVector NewPlayerPos = FVector::ZeroVector;
    std::vector<Ptr<NAV::Node>> mPath;

private:
    FORCEINLINE bool IsKeyPressed(EKeyCode InKey) const { return mInputKeyboard.IsKeyPressed(InKey); }
    FORCEINLINE bool IsKeyDown(EKeyCode InKey) const { return mInputKeyboard.IsKeyDown(InKey); }
    XKeyboardMouse mInputKeyboard;
};

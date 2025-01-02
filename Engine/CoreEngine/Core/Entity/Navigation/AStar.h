#pragma once
#include <queue>
#include "common_include.h"
#include "Node.h"
#include "Core/Input/XKeyboardMouse.h"
#include "Path.h"

struct CompareNode
{
    bool operator()(const std::shared_ptr<Nav::Node>& a, const std::shared_ptr<Nav::Node>& b) const
    {
        int aFCost = a->FCost();
        int bFCost = b->FCost();
        return aFCost > bFCost || (aFCost == bFCost && a->HCost > b->HCost);
    }
};

using PriorityQueue = std::priority_queue<Ptr<Nav::Node>, std::vector<Ptr<Nav::Node>>, CompareNode>;
using UnOrdSet = std::unordered_set<Ptr<Nav::Node>>;

class AStar
{
public:
    AStar();
    ~AStar();
    
    bool FindPath(Ptr<Nav::Node> Start, Ptr<Nav::Node> Target, float Weight);
    int GetHeuristic(Ptr<Nav::Node> A, Ptr<Nav::Node> B);
    int GetDistance(Ptr<Nav::Node> A, Ptr<Nav::Node> B);
    void RetracePath(Ptr<Nav::Node> Start, Ptr<Nav::Node> Target);
    std::vector<Ptr<Nav::Node>> simplifyPath(const std::vector<Ptr<Nav::Node>> &path);
    bool IsLineBlocked(FVector2 prevGrid, FVector2 nextGrid, std::vector<std::vector<Ptr<Nav::Node>>>& graph);

    float mSpeed = 300;
    float mRotateSpeed = 10;
    float TurnDst = 1;
    bool IsPosUpdated = false;
    FVector NewPlayerPos = FVector::ZeroVector;
    // std::vector<Ptr<NAV::Node>> mPath;
    Ptr<Path> mPath;
    int mPathIdx = 1;
    AActor* mOwnerActor = nullptr;
    FVector2 obstacle = FVector2::ZeroVector;
    int mLimitGCost = 500;
};



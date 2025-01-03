﻿#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "BTBuilder.h"
#include "Core/Utils/Math/Vector.h"
#include "enums.h"
#include "Core/Entity/Navigation/Node.h"

class AStar;

class BtBase : public JActorComponent,
                public std::enable_shared_from_this<BtBase>
{
public:
    BtBase(JTextView InName, AActor* InOwner);
    ~BtBase() override;

    void Initialize() override;
    void BeginPlay() override;
    void Destroy() override;
    void Tick(float DeltaTime) override;

public:
    // Action Function
    NodeStatus ChasePlayer(const UINT N);
    void FollowPath();
    
    // Decorator Function
    NodeStatus IsPlayerClose(const UINT N);
    NodeStatus Not(NodeStatus state);
    NodeStatus RandP(float p);
    NodeStatus IsPhase(int phase);

    // Just Function
    FVector RotateTowards(FVector direction, FVector rotation);
    
public:
    Ptr<AStar> PaStar;
    float mDeltaTime;
    FVector LastPlayerPos = FVector(-201, 0, 201);
    bool mIsPosUpdated = false;
    float mElapsedTime = 0;
    bool mEventStartFlag = true;
    bool NeedsPathReFind = true;
    int mPhase = 1;
    int mIdx;
    bool mHasPath = false;
    bool runningFlag = false;
    EFloorType mFloorType = EFloorType::FirstFloor;
    static bool mIsPlayGame;
	bool		isPendingKill = false;

protected:
    template <typename TBuilder>
    TBuilder& AddPhaseSubtree(TBuilder& builder, int phase);
    BTBuilder builder;
    Ptr<Bt::Node> BTRoot;

    inline static int32_t g_Index = 0;
};

#pragma once

#include "Core/Entity/Component/JActorComponent.h"
#include "BTBuilder.h"
#include "Core/Utils/Math/Vector.h"
#include "Core/Utils/Timer.h"
#include "Core/Input/XKeyboardMouse.h"
#include "enums.h"
#include "Core/Entity/Navigation/Node.h"

struct BlackBoard
{
    
};

class AStar;

class BtBase : public JActorComponent,
                public std::enable_shared_from_this<BtBase>
{
public:
    BtBase(JTextView InName, int Index);
    ~BtBase() override;

    void Initialize() override;
    void BeginPlay() override;
    void Destroy() override;

    void Tick(float DeltaTime) override;
    void SetupTree();
    void SetupTree2();
    void BBTick();

public:
    // Action Function
    NodeStatus Attack();
    NodeStatus Hit();
    NodeStatus Dead();
    NodeStatus ChasePlayer(const UINT N);
    void FollowPath();
    
    // Decorator Function
    NodeStatus IsPlayerClose(const UINT N);
    NodeStatus Not(NodeStatus state);
    NodeStatus RandP(float p);

    // Just Function
    FVector RotateTowards(FVector direction, FVector rotation);

public:
    static int BT_Idx;
    float mDeltaTime;
    FVector LastPlayerPos = FVector(-201, 0, 201);
    bool mIsPosUpdated = false;
    float mElapsedTime = 0;
    bool mEventStartFlag = true;
    FVector mVelocity;
    bool NeedsPathReFind = true;
    float mFloorHeight = 1.f;
    bool mHasPath = false;
    bool runningFlag = false;
    EFloorType mFloorType = EFloorType::FirstFloor;

private:
    int mIdx;
    Ptr<AStar> PaStar;
    template <typename TBuilder>
    TBuilder& AddPhaseSubtree(TBuilder& builder, int phase);
    BTBuilder builder;
    Ptr<Bt::Node> BTRoot;
};

int BtBase::BT_Idx = 0;
#pragma once

#include "BtBase.h"

class AEnemy;
class AStar;

class BT_Pig : public BtBase,
                public std::enable_shared_from_this<BT_Pig>
{
public:
    BT_Pig(JTextView InName, AActor* InOwner);
    ~BT_Pig() override;

    void Initialize() override;
    void BeginPlay() override;
    void Destroy() override;

    void Tick(float DeltaTime) override;
    void SetupTree();
    void SetupTree2();

public:
    // Action Function
    FVector2 FindGoal();
    bool IsNonPlayerDirection(FVector goal);
    NodeStatus RunFromPlayer(UINT Distance);
    NodeStatus SetGoal();
    NodeStatus Attack();
    NodeStatus Attack2();
    // NodeStatus JumpAttack();
    // void       MoveNPCWithJump(float jumpHeight, float duration);
    NodeStatus Hit();
    NodeStatus Dead();
    
    // Decorator Function
    NodeStatus IsIdle();
    NodeStatus IsRun();
    

    NodeStatus IsPlayerInSameNode();
    NodeStatus IsPlayerSoFar();
    NodeStatus StateIdleToRun();
    // Just Function
    

public:
    // BlackBoard BB;
    FVector     mVelocity;
    float       mFloorHeight    = 1.f;
    FVector2    BigGoalGrid = FVector2(0.f, 0.f);
    FVector2    GoalGrid = FVector2(0.f, 0.f);
    bool        bIsIdle = true;
    bool        bIsRun = false;        
    AEnemy* mOwnerEnemy;
    
private:
};

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
    void ResetBT() override;

public:
    // Action Function
    void FindGoal();
    bool IsNonPlayerDirection(FVector goal);
    NodeStatus RunFromPlayer(UINT Distance);
    NodeStatus SetGoal();
    NodeStatus CountPig();
    
    // Decorator Function

    NodeStatus IsPlayerNearAndPressE();
    // Just Function
    

public:
    FVector2    BigGoalGrid = FVector2::ZeroVector;
    FVector2    GoalGrid = FVector2::ZeroVector;
    std::vector<FVector2> Goals;
    AEnemy* mOwnerEnemy;
    
    inline static int32_t g_Count= 0;
private:
};



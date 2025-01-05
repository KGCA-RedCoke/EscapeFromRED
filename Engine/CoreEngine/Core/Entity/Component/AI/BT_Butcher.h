#pragma once

#include "BtBase.h"

class AEnemy;
class AStar;

class BT_Butcher : public BtBase,
                public std::enable_shared_from_this<BT_Butcher>
{
public:
    BT_Butcher(JTextView InName, AActor* InOwner);
    ~BT_Butcher() override;

    void Initialize() override;
    void BeginPlay() override;
    void Destroy() override;

    void Tick(float DeltaTime) override;
    void SetupTree();

public:
    // Action Function
    NodeStatus TalkTo();
    NodeStatus conversation(int idx);
    // NodeStatus JumpAttack();
    // void       MoveNPCWithJump(float jumpHeight, float duration);
    NodeStatus Hit();
    NodeStatus Dead();
    NodeStatus StateIdleToConvers();
    NodeStatus StateConversToTrace(int N);
    NodeStatus GetNextConvers();

    // Decorator Function
    NodeStatus IsIdle();
    NodeStatus IsConvers();
    NodeStatus IsTrace();
    NodeStatus IsPressedKey(int Key);
    

    // Just Function


public:
    FVector     mVelocity;
    float       mFloorHeight    = 1.f;
    bool        bIsIdle = true;
    bool        bIsConvers = false;
    bool        bIsTrace = false;
    int         conversIdx = 0;

    AEnemy* mOwnerEnemy;

private:
};

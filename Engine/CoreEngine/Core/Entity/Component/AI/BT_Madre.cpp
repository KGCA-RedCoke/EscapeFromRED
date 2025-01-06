#include "BT_Madre.h"

#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Entity/Navigation/AStar.h"
#include "imgui/imgui_internal.h"
#include "Core/Entity/Navigation/NavTest.h"
#include "Game/Src/Enemy/AEnemy.h"


#define LAMBDA(func, ...) [this]() -> NodeStatus { return func(__VA_ARGS__); }


BT_Madre::BT_Madre(JTextView InName, AActor* InOwner)
    : BtBase(InName, InOwner)
{
    SetupTree();
}

BT_Madre::~BT_Madre()
{
}

void BT_Madre::Initialize()
{
    assert(mOwnerActor);
    mOwnerEnemy = dynamic_cast<AEnemy*>(mOwnerActor);
    assert(mOwnerEnemy);
    
    JActorComponent::Initialize();
    mAttackDistance = 200;
}

void BT_Madre::BeginPlay() { JActorComponent::BeginPlay(); }

void BT_Madre::Destroy() { JActorComponent::Destroy(); }

void BT_Madre::Tick(float DeltaTime)
{
    BtBase::Tick(DeltaTime);
}

////////////////////////////////////////////////

// Action Function


void BT_Madre::ResetBT()
{
    BtBase::ResetBT();
    mOwnerEnemy = nullptr;
}

NodeStatus BT_Madre::Attack()
{
    if (mOwnerEnemy->GetEnemyState() == EEnemyState::Death)
        return NodeStatus::Failure;
    FVector rotation = RotateTowards(GetPlayerDirection(), mOwnerActor->GetLocalRotation());
    mOwnerActor->SetWorldRotation(rotation);
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
        FVector rotation = RotateTowards(GetPlayerDirection(), mOwnerActor->GetLocalRotation());
        mOwnerActor->SetWorldRotation(rotation);
        if (mEventStartFlag)
        {
            BB_ElapsedTime["Attack"] = 0.f;
            mOwnerEnemy->SetEnemyState(EEnemyState::Attack);
            mEventStartFlag = false;
        }
        runningFlag = false;
        if (BB_ElapsedTime["Attack"] > 1.f || mOwnerEnemy->GetEnemyState() != EEnemyState::Attack)
        {
            BB_ElapsedTime["Attack"] = 0.f;
            mEventStartFlag = true;
            mOwnerEnemy->SetEnemyState(EEnemyState::Idle);
            return NodeStatus::Success;
        }
        else
        {
            BB_ElapsedTime["Attack"] += mDeltaTime;
            runningFlag = true;
            return NodeStatus::Running;
        }
    }
    else
        return NodeStatus::Failure;
}

NodeStatus BT_Madre::Attack2()
{
    if (mOwnerEnemy->GetEnemyState() == EEnemyState::Death)
        return NodeStatus::Failure;
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
        FVector rotation = RotateTowards(GetPlayerDirection(), mOwnerActor->GetLocalRotation());
        mOwnerActor->SetWorldRotation(rotation);
        if (mEventStartFlag)
        {
            BB_ElapsedTime["Attack2"] = 0.f;
            mOwnerEnemy->SetEnemyState(EEnemyState::Attack);
            mEventStartFlag = false;
        }
        runningFlag = false;
        if (BB_ElapsedTime["Attack2"] > 1.0f || (mOwnerEnemy->GetEnemyState() != EEnemyState::Attack))
        {
            BB_ElapsedTime["Attack2"] = 0.f;
            mEventStartFlag = true;
            mOwnerEnemy->SetEnemyState(EEnemyState::Idle);
            return NodeStatus::Success;
        }
        else
        {
            BB_ElapsedTime["Attack2"] += mDeltaTime;
            runningFlag = true;
            return NodeStatus::Running;
        }
    }
    else
        return NodeStatus::Failure;
}

NodeStatus BT_Madre::Hit()
{
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
        runningFlag = false;
        FVector rotation = mOwnerActor->GetLocalRotation();
        rotation.x = 10.f;
        mOwnerActor->SetLocalRotation(rotation);
        if (BB_ElapsedTime["Hit"] > 0.3)
        {
            BB_ElapsedTime["Hit"] = 0;
            rotation.x = 0.f;
            mOwnerActor->SetLocalRotation(rotation);
            return NodeStatus::Success;
        }
        else
        {
            BB_ElapsedTime["Hit"] += mDeltaTime;
            runningFlag = true;
            return NodeStatus::Running;
        }
    }
    else
        return NodeStatus::Failure;
}

NodeStatus BT_Madre::Dead()
{
    if (mOwnerEnemy->GetEnemyState() == EEnemyState::Death)
    {
        isPendingKill = true;
        return NodeStatus::Success;
    }
    return NodeStatus::Failure;
}

// 단순 추적, 공격
void BT_Madre::SetupTree()
{
    BTRoot = builder
             .CreateRoot<Selector>()
                 .AddActionNode(LAMBDA(Dead))
                 .AddSequence("")
                     .AddActionNode(LAMBDA(ChasePlayer, 0))
                     .AddActionNode(LAMBDA(Attack2))
                 .EndBranch()
             .Build();
}

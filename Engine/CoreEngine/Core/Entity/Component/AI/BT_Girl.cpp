#include "BT_Girl.h"

#include "BT_Girl.h"
#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Entity/Navigation/AStar.h"
#include "imgui/imgui_internal.h"
#include "Core/Entity/Navigation/NavTest.h"
#include "Game/Src/Enemy/AEnemy.h"

#define LAMBDA(func, ...) [this]() -> NodeStatus { return func(__VA_ARGS__); }


BT_Girl::BT_Girl(JTextView InName, AActor* InOwner)
    : BtBase(InName, InOwner)
{
    SetupTree();
}

BT_Girl::~BT_Girl()
{
}

void BT_Girl::Initialize()
{
    assert(mOwnerActor);
    mOwnerEnemy = dynamic_cast<AEnemy*>(mOwnerActor);
    assert(mOwnerEnemy);
    
    JActorComponent::Initialize();
}

void BT_Girl::BeginPlay() { JActorComponent::BeginPlay(); }

void BT_Girl::Destroy() { JActorComponent::Destroy(); }

void BT_Girl::Tick(float DeltaTime)
{
    // mInputKeyboard.Update(DeltaTime);
    BtBase::Tick(DeltaTime);
}

////////////////////////////////////////////////

// Action Function

NodeStatus BT_Girl::IsPressedKey(EKeyCode Key)
{
    if (IsKeyPressed(Key))
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

NodeStatus BT_Girl::Attack()
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
            mOwnerEnemy->SetEnemyState(EEnemyState::Attack);
            mEventStartFlag = false;
        }
        runningFlag = false;
        if (mOwnerEnemy->GetEnemyState() != EEnemyState::Attack)
        {
            mEventStartFlag = true;
            return NodeStatus::Success;
        }
        else
        {
            runningFlag = true;
            return NodeStatus::Running;
        }
    }
    else
        return NodeStatus::Failure;
}

NodeStatus BT_Girl::Hit()
{
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
        runningFlag = false;
        FVector rotation = mOwnerActor->GetLocalRotation();
        rotation.x = 10.f;
        mOwnerActor->SetLocalRotation(rotation);
        if (mElapsedTime > 0.3)
        {
            mElapsedTime = 0;
            rotation.x = 0.f;
            mOwnerActor->SetLocalRotation(rotation);
            return NodeStatus::Success;
        }
        else
        {
            mElapsedTime += mDeltaTime;
            runningFlag = true;
            return NodeStatus::Running;
        }
    }
    else
        return NodeStatus::Failure;
}

NodeStatus BT_Girl::Dead()
{
    if (mOwnerEnemy->GetEnemyState() == EEnemyState::Death)
    {
        isPendingKill = true;
        return NodeStatus::Success;
    }
    return NodeStatus::Failure;
}

// 단순 추적, 공격
void BT_Girl::SetupTree()
{
    BTRoot = builder
             .CreateRoot<Selector>()
                 .AddActionNode(LAMBDA(Dead))
                 .AddSequence("")
                     .AddActionNode(LAMBDA(ChasePlayer, 0))
                 .EndBranch()
             .Build();
}

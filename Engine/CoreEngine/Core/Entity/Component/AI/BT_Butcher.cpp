#include "BT_Butcher.h"

#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Entity/Navigation/AStar.h"
#include "imgui/imgui_internal.h"
#include "Game/Src/Enemy/AEnemy.h"

#define LAMBDA(func, ...) [this]() -> NodeStatus { return func(__VA_ARGS__); }


BT_Butcher::BT_Butcher(JTextView InName, AActor* InOwner)
    : BtBase(InName, InOwner)
{
    SetupTree();
}

BT_Butcher::~BT_Butcher()
{
}

void BT_Butcher::Initialize()
{
    assert(mOwnerActor);
    mOwnerEnemy = dynamic_cast<AEnemy*>(mOwnerActor);
    assert(mOwnerEnemy);
    
    JActorComponent::Initialize();
}

void BT_Butcher::BeginPlay() { JActorComponent::BeginPlay(); }

void BT_Butcher::Destroy() { JActorComponent::Destroy(); }

void BT_Butcher::Tick(float DeltaTime)
{
    BtBase::Tick(DeltaTime);
}

////////////////////////////////////////////////

// Action Function

NodeStatus BT_Butcher::IsPressedKey(int Key)
{
    static bool wasKeyDown = false; // 이전 프레임에서의 상태 저장
    if (GetAsyncKeyState(Key) & 0x8000)
    {
        if (!wasKeyDown)
            wasKeyDown = true;
    }
    else if (wasKeyDown)
    {
        wasKeyDown = false;
        return NodeStatus::Success;
    }
    return NodeStatus::Failure;
}

NodeStatus BT_Butcher::StateIdleToConvers()
{
    bIsIdle = false;
    bIsConvers = true;
    mEventStartFlag = true;
    return NodeStatus::Success;
}

NodeStatus BT_Butcher::StateConversToTrace(int N)
{
    mEventStartFlag = true;
    if (conversIdx == N)
    {
        bIsConvers = false;
        bIsTrace = true;
        mOwnerEnemy->SetEnemyState(EEnemyState::Idle);
    }
    return NodeStatus::Success;
}

NodeStatus BT_Butcher::GetNextConvers()
{
    conversIdx++;
    mEventStartFlag = true;
    return NodeStatus::Success;
}

NodeStatus BT_Butcher::IsIdle()
{
    if (bIsIdle)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

NodeStatus BT_Butcher::IsConvers()
{
    if (bIsConvers)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

NodeStatus BT_Butcher::IsTrace()
{
    if (bIsTrace)
    return NodeStatus::Success;
else
    return NodeStatus::Failure;
}

NodeStatus BT_Butcher::TalkTo()
{
    if(IsPlayerClose(300))
    {
        if(mEventStartFlag)
        {
            mEventStartFlag = false;
            LOG_CORE_INFO("Press E");
        }
        return NodeStatus::Success;
    }
    mEventStartFlag = true;
    return NodeStatus::Failure;
}

NodeStatus BT_Butcher::conversation(int idx)
{
    switch (idx)
    {
    case 0:
        {
            if(mEventStartFlag)
            {
                mEventStartFlag = false;
                mOwnerEnemy->SetEnemyState(EEnemyState::Convers1);
                LOG_CORE_INFO("Welcome Traveler");
            }
            return NodeStatus::Success;
        }
        break;
    case 1:
        {
            if(mEventStartFlag)
            {
                mEventStartFlag = false;
                mOwnerEnemy->SetEnemyState(EEnemyState::Convers2);
                LOG_CORE_INFO("Bring me 10 pigs");
            }
            return NodeStatus::Success;
        }
        break;
    case 2:
        {
            if(mEventStartFlag)
            {
                mEventStartFlag = false;
                mOwnerEnemy->SetEnemyState(EEnemyState::Convers3);
                LOG_CORE_INFO("Then the Curse would disappear");
            }
            return NodeStatus::Success;
        }
        break;
    case 3:
        {
            if(mEventStartFlag)
            {
                mEventStartFlag = false;
                mOwnerEnemy->SetEnemyState(EEnemyState::Convers4);
                LOG_CORE_INFO("Good Luck!");
            }
            return NodeStatus::Success;
        }
        break;
    }
    
    return NodeStatus::Failure;
}

// 단순 추적, 공격
void BT_Butcher::SetupTree()
{
    BTRoot = builder
            .CreateRoot<Selector>()
                .AddDecorator(LAMBDA(IsIdle))
                    .AddActionNode(LAMBDA(TalkTo))
                    .AddActionNode(LAMBDA(IsPressedKey, 'E'))
                    .AddActionNode(LAMBDA(StateIdleToConvers))
                .EndBranch()
                .AddDecorator(LAMBDA(IsConvers))
                    .AddActionNode(LAMBDA(conversation, conversIdx))
                    .AddActionNode(LAMBDA(IsPressedKey, VK_SPACE))
                    .AddActionNode(LAMBDA(GetNextConvers))
                    .AddActionNode(LAMBDA(StateConversToTrace, 4))
                    .EndBranch()
                .AddDecorator(LAMBDA(IsTrace))
                    .AddActionNode(LAMBDA(ChasePlayer, 0))
                    // .AddActionNode(LAMBDA(StateTraceToIdle))
                .EndBranch()
            .Build();
}



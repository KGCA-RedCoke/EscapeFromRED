#include "BT_BOSS.h"

#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Entity/Navigation/AStar.h"
#include "imgui/imgui_internal.h"
#include "Core/Entity/Navigation/NavTest.h"
#include "Game/Src/Boss/AKillerClown.h"
#include "Game/Src/Enemy/AEnemy.h"


#define MAG 0.01f
#define LAMBDA(func, ...) [this]() -> NodeStatus { return func(__VA_ARGS__); }


BT_BOSS::BT_BOSS(JTextView InName, AActor* InOwner)
    : BtBase(InName, InOwner)
{
    // mInputKeyboard.Initialize();
    SetupTree();
}

BT_BOSS::~BT_BOSS()
{
}

void BT_BOSS::Initialize()
{
    assert(mOwnerActor);
    mOwnerEnemy = dynamic_cast<AKillerClown*>(mOwnerActor);
    assert(mOwnerEnemy);
    
    JActorComponent::Initialize();
}

void BT_BOSS::BeginPlay() { JActorComponent::BeginPlay(); }

void BT_BOSS::Destroy() { JActorComponent::Destroy(); }

void BT_BOSS::Tick(float DeltaTime)
{
    // mInputKeyboard.Update(DeltaTime);
    BtBase::Tick(DeltaTime);
    // LOG_CORE_INFO("Phase : {}",mPhase);
    if (mPhase == 2)
    {
        PaStar->mSpeed = 600;
        IsRun = true;
    }
}

////////////////////////////////////////////////

// Action Function


NodeStatus BT_BOSS::Attack1()
{
    if (mOwnerEnemy->GetBossState() == EBossState::Death)
        return NodeStatus::Failure;
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
        FVector rotation = RotateTowards(GetPlayerDirection(), mOwnerActor->GetLocalRotation());
        mOwnerActor->SetWorldRotation(rotation);
        if (mEventStartFlag)
        {
            mOwnerEnemy->SetEnemyState(EBossState::Attack1);
            mEventStartFlag = false;
        }
        runningFlag = false;
        if (mOwnerEnemy->GetBossState() != EBossState::Attack1)
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

NodeStatus BT_BOSS::Attack2()
{
    if (mOwnerEnemy->GetBossState() == EBossState::Death)
        return NodeStatus::Failure;
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
        FVector rotation = RotateTowards(GetPlayerDirection(), mOwnerActor->GetLocalRotation());
        mOwnerActor->SetWorldRotation(rotation);
        if (mEventStartFlag)
        {
            mOwnerEnemy->SetEnemyState(EBossState::Attack2);
            mEventStartFlag = false;
        }
        runningFlag = false;
        if (mOwnerEnemy->GetBossState() != EBossState::Attack2)
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
NodeStatus BT_BOSS::JumpAttack()
{
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
        // 플레이어 방향으로 회전
        FVector rotation = RotateTowards(GetPlayerDirection(), mOwnerActor->GetLocalRotation());
        mOwnerActor->SetWorldRotation(rotation);

        runningFlag = false;  // running 상태 초기화
        NeedsPathReFind = true;

        if (mEventStartFlag)
        {
            // 점프 시작
            MoveNPCWithJump(800.f, 1.0f);  // 점프 높이와 지속 시간
            mOwnerEnemy->SetEnemyState(EBossState::JumpAttack);
            mEventStartFlag = false;      // 이벤트 시작 플래그 리셋
        }
        // NPC가 바닥에 도달했는지 확인
        FVector position = mOwnerActor->GetWorldLocation();
        float height = GetFloorHeight();
        if (position.y <= height + 10.f)
        {
            if (mOwnerEnemy->GetBossState() != EBossState::JumpAttack)
            {
                mEventStartFlag = true;  // 점프를 다시 시작할 수 있도록 설정
                return NodeStatus::Success;
            }
        }
        if (GetYVelocity() > 700.f)
            mVelocity = FVector::ZeroVector;
        mOwnerActor->AddLocalLocation(mVelocity * mDeltaTime);

        runningFlag = true;  // 점프가 아직 진행 중임을 표시
        return NodeStatus::Running;
    }
    else
        return NodeStatus::Failure;
}

void BT_BOSS::MoveNPCWithJump(float jumpHeight, float duration)
{
    // 플레이어와 NPC의 거리 계산 (x, z 평면)
    FVector PlayerPos = GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation();
    FVector location = mOwnerActor->GetWorldLocation();
    float dx = PlayerPos.x - location.x;
    float dz = PlayerPos.z - location.z;

    // 이동 속도 계산
    float vx = dx / duration;
    float vz = dz / duration;

    // 초기 y축 속도 설정 (점프 궤적을 위한 수직 속도)
    float initialVerticalVelocity = (jumpHeight) / duration;
    
    // NPC의 초기 속도 설정
    SetYVelocity(initialVerticalVelocity);
    LOG_CORE_INFO("BTBOSS Jump set Y vel {}", initialVerticalVelocity);
    mVelocity = FVector(vx, 0, vz);
    // mOwnerActor->AddLocalLocation(mVelocity);
    // 속도 초기화 후 호출부에서 위치 업데이트는 다른 함수가 처리
}

NodeStatus BT_BOSS::Hit()
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

NodeStatus BT_BOSS::IsDead()
{
    
    if (mOwnerEnemy->GetBossState() == EBossState::Death
        || mOwnerEnemy->GetBossState() == EBossState::StandUp)
    {
        // isPendingKill = true;
        return NodeStatus::Success;
    }
    return NodeStatus::Failure;
}


// 보스 패턴

void BT_BOSS::SetupTree()
{
    BTRoot = builder
            .CreateRoot<Sequence>()
                .AddActionNode(LAMBDA(IsPlayerClose))
                .AddSelector("phase")
                    .AddActionNode(LAMBDA(IsDead))
                    .AddDecorator(LAMBDA(RandTime, "JumpAttack", 2.f, 0.5f))
                        .AddActionNode(LAMBDA(JumpAttack))
                    .EndBranch()
                    .AddSequence("")
                        .AddActionNode(LAMBDA(ChasePlayer, 0))
                    .EndBranch()
                .EndBranch()
            .Build();
}


void BT_BOSS::SetupTree2()
{
    BTRoot = builder
            .CreateRoot<Sequence>()
                .AddActionNode(LAMBDA(IsPlayerClose))
                .AddSelector("phase")
    #pragma region Phase1
                    .AddDecorator(LAMBDA(IsPhase, 1))
                        .AddSelector("Action")
                            .AddActionNode(LAMBDA(IsDead))
                            .AddDecorator(LAMBDA(RandTime, "JumpAttack", 2.f, 0.5f))
                                .AddActionNode(LAMBDA(JumpAttack))
                            .EndBranch()
                            .AddSequence("")
                                .AddActionNode(LAMBDA(ChasePlayer, 0))
        #pragma region          .RandAttack
                                .AddSelector("RandomSelector")
                                    .AddDecorator(LAMBDA(RandP, 0.5f))
                                        .AddActionNode(LAMBDA(Attack1))
                                    .EndBranch()
                                    .AddDecorator(LAMBDA(RandP, 1.0f))
                                        .AddActionNode(LAMBDA(Attack2))
                                    .EndBranch()
                                .EndBranch()
        #pragma endregion
                            .EndBranch()
                        .EndBranch()
                    .EndBranch()
        #pragma endregion
        #pragma region Phase2
                    .AddDecorator(LAMBDA(IsPhase, 2))
                        .AddSelector("Phase2")
                            .AddActionNode(LAMBDA(IsDead))
                            .AddDecorator(LAMBDA(RandTime, "JumpAttack", 2.f, 0.5f))
                                .AddActionNode(LAMBDA(JumpAttack))
                            .EndBranch()
                            .AddSequence("")
                                .AddActionNode(LAMBDA(ChasePlayer, 0))
                    #pragma region          .RandAttack
                                .AddSelector("RandomSelector")
                                    .AddDecorator(LAMBDA(RandP, 0.333f))
                                        .AddActionNode(LAMBDA(Attack1))
                                    .EndBranch()
                                    .AddDecorator(LAMBDA(RandP, 0.667f))
                                        .AddActionNode(LAMBDA(Attack2))
                                    .EndBranch()
                                    .AddDecorator(LAMBDA(RandP, 1.0f))
                                        .AddActionNode(LAMBDA(JumpAttack))
                                    .EndBranch()
                                .EndBranch()
                    #pragma endregion
                            .EndBranch()
                        .EndBranch()
                    .EndBranch()
        #pragma endregion
                .EndBranch()
            .Build();
}

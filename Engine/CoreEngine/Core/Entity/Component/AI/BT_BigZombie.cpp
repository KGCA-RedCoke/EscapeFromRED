#include "BT_BigZombie.h"

#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Entity/Navigation/AStar.h"
#include "imgui/imgui_internal.h"
#include "Core/Entity/Navigation/NavTest.h"
#include "Game/Src/Enemy/AEnemy.h"


#define MAG 0.01f
#define LAMBDA(func, ...) [this]() -> NodeStatus { return func(__VA_ARGS__); }


BT_BigZombie::BT_BigZombie(JTextView InName, AActor* InOwner)
    : BtBase(InName, InOwner)
{
    // mInputKeyboard.Initialize();
    SetupTree2();
}

BT_BigZombie::~BT_BigZombie()
{
}

void BT_BigZombie::Initialize()
{
    assert(mOwnerActor);
    mOwnerEnemy = dynamic_cast<AEnemy*>(mOwnerActor);
    assert(mOwnerEnemy);
    
    JActorComponent::Initialize();
}

void BT_BigZombie::BeginPlay() { JActorComponent::BeginPlay(); }

void BT_BigZombie::Destroy() { JActorComponent::Destroy(); }

void BT_BigZombie::Tick(float DeltaTime)
{
    // mInputKeyboard.Update(DeltaTime);
    BtBase::Tick(DeltaTime);
}

////////////////////////////////////////////////

// Action Function


NodeStatus BT_BigZombie::Attack()
{
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
        runningFlag = false;
        FVector rotation = mOwnerActor->GetLocalRotation();
        rotation.y += mDeltaTime * PaStar->mRotateSpeed * 100;
        mOwnerActor->SetLocalRotation(rotation);
        if (mElapsedTime > 0.5)
        {
            mElapsedTime = 0;
            // mOwnerEnemy->SetEnemyState(EEnemyState::Attack);
            return NodeStatus::Success;
        }
        else
        {
            mElapsedTime += mDeltaTime;
            runningFlag = true;
            // mOwnerEnemy->SetEnemyState(EEnemyState::Attack);
            return NodeStatus::Running;
        }
    }
    else
        return NodeStatus::Failure;
}

NodeStatus BT_BigZombie::Attack2()
{
    if (mOwnerEnemy->GetEnemyState() == EEnemyState::Death)
        return NodeStatus::Failure;
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
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
NodeStatus BT_BigZombie::JumpAttack()
{
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
        runningFlag = false;
        NeedsPathReFind = true;
        float speed = 2;
        float GRAVITY = -500.f * speed * speed;
        if (mEventStartFlag)
        {
            MoveNPCWithJump(500.f, 2.0f / speed);
            mEventStartFlag = false;
        }
        FVector position = mOwnerActor->GetWorldLocation();
        position.x += mVelocity.x * mDeltaTime;
        position.z += mVelocity.z * mDeltaTime;

        // y축 업데이트 (중력 적용)
        mVelocity.y += GRAVITY * mDeltaTime;
        position.y += mVelocity.y * mDeltaTime;

        mOwnerActor->SetWorldLocation(position);

        FVector rotation = RotateTowards(mVelocity, mOwnerActor->GetLocalRotation());
        mOwnerActor->SetLocalRotation(rotation);

        // 바닥 충돌 처리 (y축이 0 이하로 내려가지 않도록)
        if (position.y < mFloorHeight)
        {
            position.y = mFloorHeight;
            mOwnerActor->SetWorldLocation(position);
            mVelocity.y = 0.0f; // 점프 종료
            mEventStartFlag = true;
            return NodeStatus::Success;
        }
        runningFlag = true;
        return NodeStatus::Running;
    }
    else
        return NodeStatus::Failure;
}

void BT_BigZombie::MoveNPCWithJump(float jumpHeight, float duration)
{
    // NPC에서 플레이어까지의 거리 계산 (x, z 평면)
    FVector PlayerPos = GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation();
    FVector location = mOwnerActor->GetWorldLocation();
    float dx = PlayerPos.x - location.x;
    float dz = PlayerPos.z - location.z;

    // 매 프레임 NPC의 이동 속도 계산
    float vx = (dx / duration);
    float vz = (dz / duration);

    // 초기 y축 속도 계산 (포물선 형태를 위한 수직 속도)
    float initialVerticalVelocity = (2 * jumpHeight) / duration;

    // NPC의 속도 설정
    mVelocity = FVector(vx, initialVerticalVelocity, vz);
}

NodeStatus BT_BigZombie::Hit()
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

NodeStatus BT_BigZombie::Dead()
{
    if (mOwnerEnemy->GetEnemyState() == EEnemyState::Death)
    {
        isPendingKill = true;
        return NodeStatus::Success;
    }
    return NodeStatus::Failure;
}

NodeStatus BT_BigZombie::ChasePlayer(UINT N)
{
    FVector PlayerPos = GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation();
    FVector NpcPos = mOwnerActor->GetWorldLocation();
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx)
    {
        FVector2 playerGrid = G_NAV_MAP.GridFromWorldPoint(PlayerPos);
        FVector2 npcGrid = G_NAV_MAP.GridFromWorldPoint(NpcPos);
        Ptr<Nav::Node>& PlayerNode = (PlayerPos.y < 600.f)
                                         ? G_NAV_MAP.mGridGraph[playerGrid.y][playerGrid.x]
                                         : G_NAV_MAP.m2ndFloor[playerGrid.y][playerGrid.x];
        Ptr<Nav::Node>& NpcNode = (NpcPos.y < 300 || mFloorType == EFloorType::FirstFloor)
                                      ? G_NAV_MAP.mGridGraph[npcGrid.y][npcGrid.x]
                                      : G_NAV_MAP.m2ndFloor[npcGrid.y][npcGrid.x];
        if ((PlayerPos - LastPlayerPos).Length() >= 50 ||
            NeedsPathReFind)
        {
            LastPlayerPos = PlayerPos;
            if (PlayerNode->Walkable)
            {
                mHasPath = PaStar->FindPath(NpcNode,
                                            PlayerNode,
                                            2);
                NeedsPathReFind = false;
                // PaStar->mSpeed = FMath::GenerateRandomFloat(300, 800);
                PaStar->mSpeed = 300.f;
            }
        }
    }
    if (PaStar->mPath)
    {
        std::vector<Ptr<Nav::Node>> TempPath = PaStar->mPath->lookPoints;

        // auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
        // G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
        // for (auto node : TempPath)
        // {
        //     G_NAV_MAP.DrawNode(node->GridPos, Colors::Cyan);
        // }
        // G_DebugBatch.PostRender();

        if (TempPath.size() && PaStar->mPathIdx < TempPath.size())
        {
            if ((PlayerPos - mOwnerActor->GetWorldLocation()).Length() < 200) // 플레이어와 거리가 가까울 때 success
                return NodeStatus::Success;
            FollowPath();
        }
    }
    return NodeStatus::Failure;
}

void BT_BigZombie::FollowPath()
{
    FVector NextPos = PaStar->mPath->lookPoints.at(PaStar->mPathIdx)->WorldPos;
    FVector currentPos = mOwnerActor->GetWorldLocation();
    FVector direction = FVector(NextPos.x - currentPos.x, mFloorHeight - currentPos.y, NextPos.z - currentPos.z);
    if (PaStar->mPath->turnBoundaries.at(PaStar->mPathIdx)->HasCrossedLine(Path::V3ToV2(currentPos)))
    {
        mFloorType = PaStar->mPath->lookPoints.at(PaStar->mPathIdx)->OwnerFloor;
        PaStar->mPathIdx++;
    }
    else
    {
        FVector rotation = RotateTowards(direction, mOwnerActor->GetLocalRotation());
        mOwnerActor->SetLocalRotation(rotation);

        FVector velocity = FVector::ZeroVector;

        float rotationRadians = rotation.y * M_PI / 180.0f;
        velocity += FVector(-sin(rotationRadians), direction.y / 10, -cos(rotationRadians));
        velocity *= PaStar->mSpeed * mDeltaTime;
        FVector resultPos = currentPos + velocity;
        mOwnerActor->SetWorldLocation(resultPos);

        auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
        G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
        G_DebugBatch.DrawRay_Implement(currentPos, 100 * velocity, false, Colors::BlueViolet);
        G_DebugBatch.PostRender();
    }
}

NodeStatus BT_BigZombie::IsPlayerClose(const UINT N)
{
    if (PaStar->mPath && PaStar->mPathIdx < PaStar->mPath->lookPoints.size() - N)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

NodeStatus BT_BigZombie::Not(NodeStatus state)
{
    if (state == NodeStatus::Success)
        return NodeStatus::Failure;
    else if (state == NodeStatus::Failure)
        return NodeStatus::Success;
    else
        return state;
}

NodeStatus BT_BigZombie::RandP(float p)
{
    float num = FMath::GenerateRandomFloat(0.f, 1.f);
    if (num < p)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

NodeStatus BT_BigZombie::IsPhase(int phase)
{
    if (phase == mPhase)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

FVector BT_BigZombie::RotateTowards(FVector direction, FVector rotation)
{
    FVector NormalDirection;
    direction.Normalize(NormalDirection);

    float theta = atan2(NormalDirection.x, NormalDirection.z);
    float degree = theta * (180.0f / M_PI);

    float targetAngle = degree + 180.0f;
    float angleDifference = targetAngle - rotation.y;

    // 각도 차이를 -180도에서 180도 사이로 정규화
    if (angleDifference > 180.0f)
        angleDifference -= 360.0f;
    else if (angleDifference < -180.0f)
        angleDifference += 360.0f;

    if (angleDifference > 0)
        rotation.y += mDeltaTime * PaStar->mRotateSpeed * abs(angleDifference);
    else
        rotation.y -= mDeltaTime * PaStar->mRotateSpeed * abs(angleDifference); // 반시계 방향 회전

    // 각도를 0도에서 360도 사이로 정규화
    if (rotation.y >= 360.0f)
        rotation.y -= 360.0f;
    else if (rotation.y < 0.0f)
        rotation.y += 360.0f;

    return rotation;
}


// 보스 패턴
void BT_BigZombie::SetupTree()
{
    // 	BTRoot = builder
    // 			.CreateRoot<Selector>()
    // #pragma region Phase1
    // 			.AddDecorator(LAMBDA(IsPhase, 1))
    // 			.AddSelector("")
    // 			.AddDecorator(LAMBDA(IsPressedKey, EKeyCode::Space))
    // 			.AddActionNode(LAMBDA(JumpAttack))
    // 			.EndBranch()
    // 			.AddDecorator(LAMBDA(IsPressedKey, EKeyCode::V))
    // 			.AddActionNode(LAMBDA(Dead))
    // 			.EndBranch()
    // 			// .AddDecorator(LAMBDA(RandP, 0.005f))
    // 			//     .AddActionNode(LAMBDA(JumpAttack))
    // 			// .EndBranch()
    // 			.AddSequence("")
    // 			.AddActionNode(LAMBDA(ChasePlayer, 0))
    // #pragma region              Attak
    // 			.AddSelector("")
    // 			.AddDecorator(LAMBDA(RandP, 0.5f))
    // 			.AddActionNode(LAMBDA(Attack))
    // 			.EndBranch()
    // 			.AddDecorator(LAMBDA(RandP, 1.0f))
    // 			.AddActionNode(LAMBDA(Attack2))
    // 			.EndBranch()
    // 			.EndBranch()
    // #pragma endregion
    // 			.EndBranch()
    // 			.EndBranch()
    // 			.EndBranch()
    // #pragma endregion
    // #pragma region Phase2
    // 			.AddDecorator(LAMBDA(IsPhase, 2))
    // 			.AddSelector("")
    // 			.AddDecorator(LAMBDA(IsPressedKey, EKeyCode::Space))
    // 			.AddActionNode(LAMBDA(Hit))
    // 			.EndBranch()
    // 			.AddDecorator(LAMBDA(IsPressedKey, EKeyCode::V))
    // 			.AddActionNode(LAMBDA(Dead))
    // 			.EndBranch()
    // 			.AddDecorator(LAMBDA(RandP, 0.0005f))
    // 			.AddActionNode(LAMBDA(JumpAttack))
    // 			.EndBranch()
    // 			.AddSequence("")
    // 			.AddActionNode(LAMBDA(ChasePlayer, 1))
    // #pragma region Attak
    // 			.AddSelector("")
    // 			.AddDecorator(LAMBDA(RandP, 0.5f))
    // 			.AddActionNode(LAMBDA(Attack))
    // 			.EndBranch()
    // 			.AddActionNode(LAMBDA(Attack2))
    // 			.EndBranch()
    // #pragma endregion
    // 			.EndBranch()
    // 			.EndBranch()
    // 			.EndBranch()
    // #pragma endregion
    // 			.Build();
}

// 단순 추적, 공격
void BT_BigZombie::SetupTree2()
{
    BTRoot = builder
             .CreateRoot<Selector>()
                 .AddActionNode(LAMBDA(Dead))
                 .AddSequence("")
                    // .AddActionNode(LAMBDA(IsPressedKey, EKeyCode::Space))
                     .AddActionNode(LAMBDA(ChasePlayer, 0))
                     .AddActionNode(LAMBDA(Attack2))
                 .EndBranch()
             .Build();
}

#include "BT_BOSS.h"

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


BT_BOSS::BT_BOSS(JTextView InName, AActor* InOwner)
    : JActorComponent(InName, InOwner),
      mIdx(g_Index++)
{
    PaStar = MakePtr<AStar>();
    SetupTree2();
}

BT_BOSS::~BT_BOSS()
{
}

void BT_BOSS::Initialize()
{
    assert(mOwnerActor);
    mOwnerEnemy = dynamic_cast<AEnemy*>(mOwnerActor);
    assert(mOwnerEnemy);
    
    JActorComponent::Initialize();
}

void BT_BOSS::BeginPlay() { JActorComponent::BeginPlay(); }

void BT_BOSS::Destroy() { JActorComponent::Destroy(); }

void BT_BOSS::Tick(float DeltaTime)
{
    JActorComponent::Tick(DeltaTime);
    BBTick();
    mDeltaTime = DeltaTime;
    if (GetWorld.bGameMode)
        BTRoot->tick();
}

void BT_BOSS::BBTick()
{
    // FVector2 playerGrid = G_NAV_MAP.GridFromWorldPoint(PlayerPos);
}

////////////////////////////////////////////////

// Action Function


NodeStatus BT_BOSS::Attack()
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

NodeStatus BT_BOSS::Attack2()
{
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
NodeStatus BT_BOSS::JumpAttack()
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

void BT_BOSS::MoveNPCWithJump(float jumpHeight, float duration)
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

NodeStatus BT_BOSS::Dead()
{
    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx || runningFlag)
    {
        runningFlag = false;
        FVector rotation = mOwnerActor->GetLocalRotation();
        rotation.x = 90.f;
        mOwnerActor->SetLocalRotation(rotation);
        if (mElapsedTime > 1)
        {
            mElapsedTime = 0;
            rotation.x = 0.f;
            mOwnerActor->SetLocalRotation(rotation);
            mPhase++;
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

NodeStatus BT_BOSS::ChasePlayer(UINT N)
{
    FVector PlayerPos = GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation();

    int frameIdx = GetWorld.currentFrame % g_Index;
    if (frameIdx == mIdx)
    {
        FVector2 playerGrid = G_NAV_MAP.GridFromWorldPoint(PlayerPos);
        FVector2 npcGrid = G_NAV_MAP.GridFromWorldPoint(mOwnerActor->GetWorldLocation());

        Ptr<Nav::Node>& PlayerNode = (G_NAV_MAP.PlayerHeight < 560.f)
                                         ? G_NAV_MAP.mGridGraph[playerGrid.y][playerGrid.x]
                                         : G_NAV_MAP.m2ndFloor[playerGrid.y][playerGrid.x];
        Ptr<Nav::Node>& NpcNode = (mFloorType == EFloorType::FirstFloor)
                                      ? G_NAV_MAP.mGridGraph[npcGrid.y][npcGrid.x]
                                      : G_NAV_MAP.m2ndFloor[npcGrid.y][npcGrid.x];
        if ((PlayerPos - LastPlayerPos).Length() >= 100 ||
            NeedsPathReFind)
        /*(PaStar->mPath->lookPoints.at(PaStar->mPathIdx)->GridPos - G_NAV_MAP.GridFromWorldPoint(mOwnerActor->GetWorldLocation())).GetLength() >= 1.5)*/
        {
            LastPlayerPos = PlayerPos;
            if (G_NAV_MAP.mGridGraph[playerGrid.y][playerGrid.x]->Walkable)
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
    // if (!mHasPath)
    //     return NodeStatus::Failure;
    if (PaStar->mPath)
    {
        std::vector<Ptr<Nav::Node>> TempPath = PaStar->mPath->lookPoints;

        auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
        G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
        for (auto node : TempPath)
        {
            G_NAV_MAP.DrawNode(node->GridPos, Colors::Cyan);
        }
        G_DebugBatch.PostRender();

        if (TempPath.size() && (PlayerPos - mOwnerActor->GetWorldLocation()).Length() > 150)
        {
            FollowPath();
            return NodeStatus::Failure;
        }
        else //if ((PlayerPos - mOwnerActor->GetWorldLocation()).Length() < 400) // 플레이어와 거리가 가까울 때 success
            return NodeStatus::Success;
    }
    return NodeStatus::Failure;
}

void BT_BOSS::FollowPath()
{
    FVector NextPos = PaStar->mPath->lookPoints.at(PaStar->mPathIdx)->WorldPos;
    FVector currentPos = mOwnerActor->GetWorldLocation();
    FVector direction = FVector(NextPos.x - currentPos.x, mFloorHeight - currentPos.y, NextPos.z - currentPos.z);
    if (PaStar->mPath->turnBoundaries.at(PaStar->mPathIdx)->HasCrossedLine(Path::V3ToV2(currentPos)))
    {
        mFloorType = PaStar->mPath->lookPoints.at(PaStar->mPathIdx)->OwnerFloor;
        PaStar->mPathIdx++;
        // mIsPosUpdated = true;
    }
    else
    {
        // mIsPosUpdated = false;
        FVector rotation = RotateTowards(direction, mOwnerActor->GetLocalRotation());
        mOwnerActor->SetLocalRotation(rotation);

        FVector velocity = FVector::ZeroVector;

        float rotationRadians = rotation.y * M_PI / 180.0f;
        velocity += FVector(-sin(rotationRadians), direction.y / 10, -cos(rotationRadians));
        velocity *= PaStar->mSpeed * mDeltaTime;
        FVector resultPos = currentPos + velocity;
        // resultPos.y = mFloorHeight;
        mOwnerActor->SetWorldLocation(resultPos);

        auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
        G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
        G_DebugBatch.DrawRay_Implement(currentPos, 100 * velocity, false, Colors::BlueViolet);
        G_DebugBatch.PostRender();
    }
}

NodeStatus BT_BOSS::IsPlayerClose(const UINT N)
{
    if (PaStar->mPath && PaStar->mPathIdx < PaStar->mPath->lookPoints.size() - N)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

NodeStatus BT_BOSS::Not(NodeStatus state)
{
    if (state == NodeStatus::Success)
        return NodeStatus::Failure;
    else if (state == NodeStatus::Failure)
        return NodeStatus::Success;
    else
        return state;
}

NodeStatus BT_BOSS::RandP(float p)
{
    float num = FMath::GenerateRandomFloat(0.f, 1.f);
    if (num < p)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

NodeStatus BT_BOSS::IsPhase(int phase)
{
    if (phase == mPhase)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

FVector BT_BOSS::RotateTowards(FVector direction, FVector rotation)
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
void BT_BOSS::SetupTree()
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
void BT_BOSS::SetupTree2()
{
    BTRoot = builder
             .CreateRoot<Selector>()
             // .AddDecorator(LAMBDA(IsPressedKey, EKeyCode::Space))
             //     .AddActionNode(LAMBDA(JumpAttack))
             // .EndBranch()
                .AddSequence("")
             //     .AddActionNode(LAMBDA(IsPressedKey, EKeyCode::Space))
                     .AddActionNode(LAMBDA(ChasePlayer, 0))
                     .AddActionNode(LAMBDA(Attack2))
                 .EndBranch()
             .Build();
}

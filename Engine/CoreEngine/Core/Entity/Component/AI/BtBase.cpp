#include "BtBase.h"

#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Entity/Navigation/AStar.h"
#include "imgui/imgui_internal.h"
#include "Core/Entity/Navigation/NavTest.h"

BtBase::BtBase(JTextView InName, AActor* InOwner)
    : JActorComponent(InName, InOwner),
      mIdx(g_Index++)
{
    PaStar = MakePtr<AStar>();
}

BtBase::~BtBase(){}

void BtBase::Initialize() { JActorComponent::Initialize(); }

void BtBase::BeginPlay() { JActorComponent::BeginPlay(); }

void BtBase::Destroy() { JActorComponent::Destroy(); }

void BtBase::Tick(float DeltaTime)
{
    JActorComponent::Tick(DeltaTime);
    mDeltaTime = DeltaTime;
    if (GetWorld.bGameMode && !isPendingKill)
        BTRoot->tick();
}

////////////////////////////////////////////////

// Action Function

NodeStatus BtBase::ChasePlayer(UINT N)
{
    FVector     PlayerPos = GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation();

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
                PlayerNode, 2);
                NeedsPathReFind = false;
                PaStar->mSpeed = FMath::GenerateRandomFloat(300, 800);
                // PaStar->mSpeed = 300.f;
            }
        }
    }   
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
        
        if (TempPath.size() && PaStar->mPathIdx < TempPath.size() - N)
        {
            FollowPath();
            return NodeStatus::Failure;
        }
        else if ((PlayerPos - mOwnerActor->GetWorldLocation()).Length() < 400) // 플레이어와 거리가 가까울 때 success
            return NodeStatus::Success;
    }
    return NodeStatus::Failure;
}


NodeStatus BtBase::IsPhase(int phase)
{
    if (phase == mPhase)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

void BtBase::FollowPath()
{
    FVector NextPos = PaStar->mPath->lookPoints.at(PaStar->mPathIdx)->WorldPos;
    FVector currentPos = mOwnerActor->GetWorldLocation();
    FVector direction = FVector(NextPos.x - currentPos.x, 0, NextPos.z - currentPos.z);
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
        mOwnerActor->AddLocalLocation(velocity);
        
        // auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
        // G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
        // G_DebugBatch.DrawRay_Implement(currentPos, 100 * velocity
        //     , false, Colors::BlueViolet);
        // G_DebugBatch.PostRender();
    }
}

NodeStatus BtBase::IsPlayerClose(const UINT N)
{
    if (PaStar->mPath && PaStar->mPathIdx < PaStar->mPath->lookPoints.size() - N)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

NodeStatus BtBase::Not(NodeStatus state)
{
    if (state == NodeStatus::Success)
        return NodeStatus::Failure;
    else if (state == NodeStatus::Failure)
        return NodeStatus::Success;
    else
        return state;
}

NodeStatus BtBase::RandP(float p)
{
    float num = FMath::GenerateRandomFloat(0.f, 1.f);
    if (num < p)
        return NodeStatus::Success;
    else
        return NodeStatus::Failure;
}

FVector BtBase::RotateTowards(FVector direction, FVector rotation)
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
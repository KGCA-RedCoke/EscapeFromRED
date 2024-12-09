#include "AStar.h"

#include <fbxsdk/core/fbxsystemunit.h>

#include "NavTest.h"
#include "Core/Utils/Math/MathUtility.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"

void AStar::Initialize()
{
}

void AStar::BeginPlay()
{
}

void AStar::Tick(float DeltaTime)
{
    
    mInputKeyboard.Update(DeltaTime);
    PushHold = false;
    if (IsKeyPressed(EKeyCode::Space))
    {
        PushHold = !PushHold;
    }
    if (PushHold)
    {
        FVector2 playerGrid = NAV_MAP.GridFromWorldPoint(NAV_MAP.PlayerPos);
        
        if (abs(playerGrid.x - NAV_MAP.GridFromWorldPoint(NewPlayerPos).x) >= 1 ||
            abs(playerGrid.y - NAV_MAP.GridFromWorldPoint(NewPlayerPos).y) >= 1)
        {
            NewPlayerPos = NAV_MAP.PlayerPos;
            FVector2 npcGrid = NAV_MAP.GridFromWorldPoint(GetOwnerActor()->GetWorldLocation());
            // if (npcGrid.y < NAV_MAP.mGridGraph.size())
            if (NAV_MAP.mGridGraph[playerGrid.y][playerGrid.x]->Walkable)
            {
                FindPath(NAV_MAP.mGridGraph.at(npcGrid.y).at(npcGrid.x),
                NAV_MAP.mGridGraph.at(playerGrid.y).at(playerGrid.x));
                mSpeed = FMath::GenerateRandomFloat(300, 800);
            }
        }
        if (mPath.size() > 1)
            FollowPath(DeltaTime);
        else
        {
            FVector rotation = GetOwnerActor()->GetLocalRotation();
            rotation.y += DeltaTime * mRotateSpeed * 5;
            GetOwnerActor()->SetLocalRotation(rotation);
        }
    }
    auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
    G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
    for (auto grid : mPath)
    {
        NAV_MAP.DrawNode(FVector2(grid->GridX, grid->GridY), Colors::Cyan);
    }
    G_DebugBatch.PostRender();
}

void AStar::Destroy()
{
}

void AStar::FindPath(Ptr<Node> Start, Ptr<Node> Target)
{
    // PriorityQueue openSet;
    std::vector<Ptr<Node>> openSet;
    UnOrdSet closedSet;
    openSet.push_back(Start);
    std::push_heap(openSet.begin(), openSet.end(), CompareNode());

    while (!openSet.empty())
    {
        Ptr<Node> current = openSet.front();
        std::pop_heap(openSet.begin(), openSet.end(), CompareNode());
        openSet.pop_back();
        closedSet.insert(current);
        if (current == Target)
        {
            RetracePath(Start, Target);
            return;
        }
        for (auto child : current->Children)
        {
            if (closedSet.find(child.lock()) != closedSet.end())
                continue;
            int newMoveCostToChild = current->GCost + GetDistance(current, child.lock());
            if (newMoveCostToChild < child.lock()->GCost)
            {
                child.lock()->GCost = newMoveCostToChild;
                child.lock()->HCost = GetDistance(current, Target);
                child.lock()->Parent = current;
            }
            if (std::find(openSet.begin(), openSet.end(), child.lock()) == openSet.end())
            {
                child.lock()->GCost = newMoveCostToChild;
                child.lock()->HCost = GetDistance(current, Target);
                child.lock()->Parent = current;
                openSet.push_back(child.lock());
                std::push_heap(openSet.begin(), openSet.end(), CompareNode());
            }
        }
    }
}

int AStar::GetDistance(Ptr<Node> A, Ptr<Node> B)
{
    int dstX = abs(A->GridX - B->GridX);
    int dstY = abs(A->GridY - B->GridY);
    if (dstX > dstY)
        return 14 * dstY + 10 * (dstX - dstY);
    return 14 * dstX + 10 * (dstY - dstX);
}

void AStar::RetracePath(Ptr<Node> Start, Ptr<Node> Target)
{
    mPath.clear();
    Ptr<Node> current = Target;
    while (current != Start)
    {
        mPath.push_back(current);
        current = current->Parent.lock();
    }
    NAV_MAP.tempPath = mPath;
}

void AStar::FollowPath(float DeltaTime)
{
    FVector NextPos = mPath.back()->WorldPos;
    FVector currentPos = GetOwnerActor()->GetWorldLocation();
    FVector direction = NextPos - currentPos + NAV_MAP.NodeCenter;
    if (abs(direction.Length()) < 10)
    {
        if (!IsPosUpdated)
        {
            mPath.pop_back();
            IsPosUpdated = true;
            
            // std::cout << "nextNode : x = " << NextPos.x << " y = " << NextPos.y <<
            //     "  direction : x = " << direction.x << " z = " << direction.z << std::endl;
        }
    }
    else
    {
        IsPosUpdated = false;
        FVector location = GetOwnerActor()->GetLocalLocation();
        FVector NormalDirection;
        direction.Normalize(NormalDirection);
        location += NormalDirection * mSpeed * DeltaTime;
        GetOwnerActor()->SetLocalLocation(location);

        float theta = atan2(NormalDirection.x, NormalDirection.z);
        float degree = theta * (180.0f / M_PI);
        
        FVector rotation = GetOwnerActor()->GetLocalRotation();
        float targetAngle = degree + 180.0f;
        float angleDifference = targetAngle - rotation.y;

        // 각도 차이를 -180도에서 180도 사이로 정규화
        if (angleDifference > 180.0f)
            angleDifference -= 360.0f;
        else if (angleDifference < -180.0f)
            angleDifference += 360.0f;

        if (angleDifference > 0)
            rotation.y += DeltaTime * mRotateSpeed; // 시계 방향 회전
        else
            rotation.y -= DeltaTime * mRotateSpeed; // 반시계 방향 회전

        // 각도를 0도에서 360도 사이로 정규화
        if (rotation.y >= 360.0f)
            rotation.y -= 360.0f;
        else if (rotation.y < 0.0f)
            rotation.y += 360.0f;
        
        GetOwnerActor()->SetLocalRotation(rotation);
    }
}

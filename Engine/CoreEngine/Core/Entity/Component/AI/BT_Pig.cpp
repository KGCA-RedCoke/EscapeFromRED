#include "BT_Pig.h"

#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Entity/Navigation/AStar.h"
#include "Core/Entity/Navigation/BigGrid.h"
#include "Core/Entity/Navigation/NavTest.h"
#include "Core/Entity/Navigation/Path.h"
#include "imgui/imgui_internal.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Game/Src/Enemy/AEnemy.h"

#define LAMBDA(func, ...) [this]() -> NodeStatus { return func(__VA_ARGS__); }


BT_Pig::BT_Pig(JTextView InName, AActor* InOwner)
	: BtBase(InName, InOwner)
{
	SetupTree();
}

BT_Pig::~BT_Pig()
{}

void BT_Pig::Initialize()
{
	assert(mOwnerActor);
	mOwnerEnemy = dynamic_cast<AEnemy*>(mOwnerActor);
	assert(mOwnerEnemy);

	JActorComponent::Initialize();

	mWorld = dynamic_cast<JLevel*>(GetWorld.LevelManager->GetActiveLevel());

	mOwnerEnemy->OnInteractionStart.Bind([this](){
		mWorld->ShowPressEKey(true);
	});

	mOwnerEnemy->OnInteractionEnd.Bind([this](){
		mWorld->ShowPressEKey(false);
	});

}

void BT_Pig::BeginPlay() { JActorComponent::BeginPlay(); }

void BT_Pig::Destroy() { JActorComponent::Destroy(); }

void BT_Pig::Tick(float DeltaTime)
{
	BtBase::Tick(DeltaTime);
	// G_BIG_MAP.Render();
	// FVector npcPos = mOwnerActor->GetWorldLocation();
	// G_NAV_MAP.Render();
	// auto* cam = GetWorld.CameraManager->GetCurrentMainCam();
	// SetGoal();
	// G_DebugBatch.PreRender(cam->GetViewMatrix(), cam->GetProjMatrix());
	// {
	//     G_BIG_MAP.DrawNode(BigGoalGrid, Colors::Cyan);
	//     G_NAV_MAP.DrawNode(GoalGrid, Colors::Pink);
	// }
	// G_DebugBatch.PostRender();
}

// Action Function

void BT_Pig::FindGoal()
{
	Goals.clear();
	FVector2       LastChildGrid = FVector2(0.0f, 0.0f);
	FVector2       NpcGrid       = G_BIG_MAP.GridFromWorldPoint(mOwnerActor->GetWorldLocation());
	Ptr<Nav::Node> NpcNode       = G_BIG_MAP.mGridGraph[NpcGrid.y][NpcGrid.x];
	for (auto child : NpcNode->Children)
	{
		if (IsNonPlayerDirection(child.lock()->WorldPos))
			Goals.push_back(child.lock()->GridPos);
	}
}

bool BT_Pig::IsNonPlayerDirection(FVector goal)
{
	FVector2 PlayerPos;
	FVector2 NpcPos;
	FVector2 GoalPos;

	PlayerPos.x = GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation().x;
	PlayerPos.y = GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation().z;
	NpcPos.x    = mOwnerActor->GetWorldLocation().x;
	NpcPos.y    = mOwnerActor->GetWorldLocation().z;
	GoalPos.x   = goal.x;
	GoalPos.y   = goal.z;

	FVector2 PlayerLookAt = NpcPos - PlayerPos;
	FVector2 MiddlePoint  = (PlayerPos + NpcPos) * 0.5f;
	FVector2 Direction    = GoalPos - MiddlePoint;

	return (PlayerLookAt | Direction) > 0.0f;
}

NodeStatus BT_Pig::RunFromPlayer(UINT Distance)
{
	FVector PlayerPos = GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation();
	FVector NpcPos    = mOwnerActor->GetWorldLocation();
	int     frameIdx  = GetWorld.currentFrame % g_Index;
	if (frameIdx == mIdx)
	{
		Ptr<Nav::Node>& GoalNode = G_NAV_MAP.mGridGraph[GoalGrid.y][GoalGrid.x];

		FVector2        npcGrid = G_NAV_MAP.GridFromWorldPoint(NpcPos);
		Ptr<Nav::Node>& NpcNode = G_NAV_MAP.mGridGraph[npcGrid.y][npcGrid.x];

		if ((PlayerPos - LastPlayerPos).Length() >= 50 || NeedsPathReFind)
		{
			LastPlayerPos   = PlayerPos;
			mHasPath        = PaStar->FindPath(NpcNode, GoalNode, 2);
			PaStar->mSpeed  = FMath::GenerateRandomFloat(500, 1000);
			NeedsPathReFind = false;
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
			if ((G_NAV_MAP.WorldPosFromGridPos(GoalGrid) - mOwnerActor->GetWorldLocation()).Length() <
				Distance) // 플레이어와 거리가 가까울 때 success
				return NodeStatus::Success;
			FollowPath();
		}
	}
	return NodeStatus::Failure;
}

NodeStatus BT_Pig::SetGoal()
{
	if (mElapsedTime > 2.f && IsPlayerClose(2000))
	{
		FindGoal();
		if (!Goals.empty())
			BigGoalGrid = Goals[FMath::GenerateNum(0, Goals.size() - 1)];
		FVector TempPos = G_BIG_MAP.WorldPosFromGridPos(BigGoalGrid);
		GoalGrid        = G_NAV_MAP.GridFromWorldPoint(TempPos);
		mElapsedTime    = 0.f;
	}
	mElapsedTime += mDeltaTime;
	return NodeStatus::Success;
}

NodeStatus BT_Pig::IsPlayerNearAndPressE()
{
	if (GetAsyncKeyState('E') & 0x8000 && IsPlayerClose(180))
	{
		mOwnerEnemy->Destroy();
		return NodeStatus::Success;
	}
	return NodeStatus::Failure;
}

NodeStatus BT_Pig::CountPig()
{
	g_Count++;
	LOG_CORE_INFO("Caught PIG : {}", g_Count);
	return NodeStatus::Success;
}

// 단순 추적
void BT_Pig::SetupTree()
{
	BTRoot = builder
			 .CreateRoot<Selector>()
				 .AddDecorator(LAMBDA(IsPlayerNearAndPressE))
					 .AddActionNode(LAMBDA(CountPig))
				 .EndBranch()
				 .AddSequence("Run")
					 .AddActionNode(LAMBDA(SetGoal))
					 .AddActionNode(LAMBDA(RunFromPlayer, 1000))
				.EndBranch()
			 .Build();
}

void BT_Pig::ResetBT()
{
	BtBase::ResetBT();
	BigGoalGrid = FVector2::ZeroVector;
	GoalGrid    = FVector2::ZeroVector;
	Goals.clear();
	mOwnerEnemy = nullptr;
}

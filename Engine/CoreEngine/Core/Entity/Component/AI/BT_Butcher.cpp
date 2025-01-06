#include "BT_Butcher.h"

#include "GUI/GUI_Inspector.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Entity/Navigation/Path.h"
#include "Core/Entity/Navigation/AStar.h"
#include "Core/Entity/Navigation/NavTest.h"
#include "imgui/imgui_internal.h"
#include "Game/Src/Enemy/AEnemy.h"
#include "Game/Src/Level/JLevel_Main.h"

#define LAMBDA(func, ...) [this]() -> NodeStatus { return func(__VA_ARGS__); }


BT_Butcher::BT_Butcher(JTextView InName, AActor* InOwner)
	: BtBase(InName, InOwner)
{
	mInputKeyboard.Initialize();
	SetupTree();
}

BT_Butcher::~BT_Butcher()
{}

void BT_Butcher::Initialize()
{
	assert(mOwnerActor);
	mOwnerEnemy = dynamic_cast<AEnemy*>(mOwnerActor);
	assert(mOwnerEnemy);

	mWorld = dynamic_cast<JLevel_Main*>(GetWorld.LevelManager->GetActiveLevel());

	mOwnerEnemy->OnInteractionStart.Bind([this](){
		mWorld->ShowPressEKey(true);
	});

	mOwnerEnemy->OnInteractionEnd.Bind([this](){
		mWorld->ShowPressEKey(false);
	});

	JActorComponent::Initialize();
	
	PaStar->mMaxGCost = 2000;
	PaStar->mMinGCost = 2000;
}

void BT_Butcher::BeginPlay() { JActorComponent::BeginPlay(); }

void BT_Butcher::Destroy() { JActorComponent::Destroy(); }

void BT_Butcher::Tick(float DeltaTime)
{
	mInputKeyboard.Update(DeltaTime);
	BtBase::Tick(DeltaTime);
}

////////////////////////////////////////////////

// Action Function

NodeStatus BT_Butcher::IsPressedKey(EKeyCode Key)
{
	static bool wasKeyDown = false; // 이전 프레임에서의 상태 저장

	if (IsKeyDown(Key))
	{
		if (!wasKeyDown)
			wasKeyDown = true;
	}
	else if (wasKeyDown && IsKeyUp(Key))
	{
		wasKeyDown = false;
		return NodeStatus::Success;
	}
	return NodeStatus::Failure;
}

NodeStatus BT_Butcher::StateIdleToConvers()
{
	bIsIdle         = false;
	bIsConvers      = true;
	mEventStartFlag = true;
	return NodeStatus::Success;
}

NodeStatus BT_Butcher::StateConversToTrace(int N)
{
	mEventStartFlag = true;
	if (conversIdx == N)
	{
		bIsConvers = false;
		bIsTrace   = true;
		mOwnerEnemy->SetEnemyState(EEnemyState::Idle);
		mWorld->OnQuestEnd.Execute(conversIdx);
	}
	return NodeStatus::Success;
}

NodeStatus BT_Butcher::GetNextConvers()
{
	conversIdx++;
	mEventStartFlag = true;
	return NodeStatus::Success;
}

NodeStatus BT_Butcher::GetPath(FVector2 GoalGrid)
{
	FVector2 npcGrid = G_NAV_MAP.GridFromWorldPoint(mOwnerActor->GetWorldLocation());
	if (PaStar->FindPath(G_NAV_MAP.mGridGraph[npcGrid.y][npcGrid.x], G_NAV_MAP.mGridGraph[GoalGrid.y][GoalGrid.x], 2))
		return NodeStatus::Success;
	return NodeStatus::Failure;
}

NodeStatus BT_Butcher::GoGoal()
{
	std::vector<Ptr<Nav::Node>> TempPath = PaStar->mPath->lookPoints;
	if (TempPath.size() && PaStar->mPathIdx < TempPath.size())
	{
		FollowPath();
		return NodeStatus::Running;
	}
	return NodeStatus::Success;
}

NodeStatus BT_Butcher::LookAt(FVector direction)
{
	if (mElapsedTime < 2.0f)
	{
		FVector rotation = mOwnerActor->GetLocalRotation();
		mOwnerActor->SetLocalRotation(RotateTowards(direction, rotation));
		return NodeStatus::Running;
	}
	bIsTrace = false;
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
	if (IsPlayerClose(300))
	{

		if (mEventStartFlag)
		{
			mEventStartFlag = false;
		}
		return NodeStatus::Success;
	}
	mEventStartFlag = true;
	return NodeStatus::Failure;
}

NodeStatus BT_Butcher::conversation(int idx)
{
	if (mEventStartFlag)
	{
		mWorld->OnQuestStart.Execute(idx);
	}

	switch (idx)
	{
	case 0:
		{
			if (mEventStartFlag)
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
			if (mEventStartFlag)
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
			if (mEventStartFlag)
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
			if (mEventStartFlag)
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
					.AddActionNode(LAMBDA(IsPressedKey, EKeyCode::E))
					.AddActionNode(LAMBDA(StateIdleToConvers))
				.EndBranch()
				.AddDecorator(LAMBDA(IsConvers))
					.AddActionNode(LAMBDA(conversation, conversIdx))
					.AddActionNode(LAMBDA(IsPressedKey, EKeyCode::Space))
					.AddActionNode(LAMBDA(GetNextConvers))
					.AddActionNode(LAMBDA(StateConversToTrace, 4))
				.EndBranch()
				.AddDecorator(LAMBDA(IsTrace))
					.AddActionNode(LAMBDA(GetPath, FVector2(50, 100)))
					.AddActionNode(LAMBDA(GoGoal))
					.AddActionNode(LAMBDA(LookAt, FVector(100, 110, 100)))
				.EndBranch()
			.Build();
}

void BT_Butcher::ResetBT(AActor* NewOwner)
{
	BtBase::ResetBT(nullptr);
	bIsIdle = true;
	bIsConvers = false;
	bIsTrace = false;
	conversIdx = 0;
	mOwnerEnemy = nullptr;
	mWorld = nullptr;
}

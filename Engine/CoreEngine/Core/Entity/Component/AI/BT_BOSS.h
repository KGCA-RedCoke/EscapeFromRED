#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "BTBuilder.h"
#include "Core/Utils/Math/Vector.h"
#include "Core/Utils/Timer.h"
#include "Core/Input/XKeyboardMouse.h"
#include "enums.h"
#include "Core/Entity/Navigation/Node.h"

class AStar;

class BT_BOSS : public JActorComponent,
				public std::enable_shared_from_this<BT_BOSS>
{
public:
	BT_BOSS(JTextView InName);
	~BT_BOSS() override;

	void Initialize() override;
	void BeginPlay() override;
	void Destroy() override;

	void Tick(float DeltaTime) override;
	void SetupTree();
	void SetupTree2();
	void BBTick();

public:
	// Action Function
	NodeStatus Attack();
	NodeStatus Attack2();
	NodeStatus JumpAttack();
	void       MoveNPCWithJump(float jumpHeight, float duration);
	NodeStatus Hit();
	NodeStatus Dead();
	NodeStatus ChasePlayer(const UINT N);
	void       FollowPath();

	// Decorator Function
	NodeStatus IsPlayerClose(const UINT N);
	NodeStatus Not(NodeStatus state);
	NodeStatus RandP(float p);
	NodeStatus IsPhase(int phase);

	// Just Function
	FVector RotateTowards(FVector direction, FVector rotation);

public:
	// BlackBoard BB;
	Ptr<AStar>  PaStar;
	float       mDeltaTime;
	FVector     LastPlayerPos   = FVector(-201, 0, 201);
	bool        mIsPosUpdated   = false;
	float       mElapsedTime    = 0;
	bool        mEventStartFlag = true;
	FVector     mVelocity;
	int         mPhase          = 1;
	bool        NeedsPathReFind = true;
	float       mFloorHeight    = 1.f;
	int         mIdx;
	bool        mHasPath    = false;
	bool        runningFlag = false;
	EFloorType  mFloorType  = EFloorType::FirstFloor;
	static bool mIsPlayGame;

private:
	template <typename TBuilder>
	TBuilder&     AddPhaseSubtree(TBuilder& builder, int phase);
	BTBuilder     builder;
	Ptr<Bt::Node> BTRoot;

	inline static int32_t g_Index = 0;
};

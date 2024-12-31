#pragma once
// #include "Core/Input/XKeyboardMouse.h"
#include "BtBase.h"

class AEnemy;
class AStar;

class BT_BOSS : public BtBase,
				public std::enable_shared_from_this<BT_BOSS>
{
public:
	BT_BOSS(JTextView InName, AActor* InOwner);
	~BT_BOSS() override;

	void Initialize() override;
	void BeginPlay() override;
	void Destroy() override;

	void Tick(float DeltaTime) override;
	void SetupTree();
	void SetupTree2();

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
	NodeStatus IsPressedKey(EKeyCode Key);

	// Just Function
	FVector RotateTowards(FVector direction, FVector rotation);

public:
	// BlackBoard BB;
	FVector     mVelocity;
	float       mFloorHeight    = 1.f;

	AEnemy* mOwnerEnemy;

private:
	// FORCEINLINE bool IsKeyPressed(EKeyCode InKey) const { return mInputKeyboard.IsKeyPressed(InKey); }
	// FORCEINLINE bool IsKeyDown(EKeyCode InKey) const { return mInputKeyboard.IsKeyDown(InKey); }
	// XKeyboardMouse mInputKeyboard;
};

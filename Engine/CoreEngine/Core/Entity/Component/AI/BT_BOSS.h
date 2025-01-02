#pragma once
// #include "Core/Input/XKeyboardMouse.h"
#include "BtBase.h"

class AKillerClown;
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
	NodeStatus Attack1();
	NodeStatus Attack2();
	NodeStatus Attack3();
	NodeStatus JumpAttack();
	void       MoveNPCWithJump(float jumpHeight, float duration);
	NodeStatus Hit();
	NodeStatus Dead();
	NodeStatus Resurrect();


public:
	// BlackBoard BB;
	bool		bIsSprinting = false;
	FVector     mVelocity;
	float       mFloorHeight    = 1.f;
	bool		bBossBattleOn = false;

	AKillerClown* mOwnerEnemy;

private:
	// FORCEINLINE bool IsKeyPressed(EKeyCode InKey) const { return mInputKeyboard.IsKeyPressed(InKey); }
	// FORCEINLINE bool IsKeyDown(EKeyCode InKey) const { return mInputKeyboard.IsKeyDown(InKey); }
	// XKeyboardMouse mInputKeyboard;
};

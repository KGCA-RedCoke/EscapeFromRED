#pragma once
// #include "Core/Input/XKeyboardMouse.h"
#include "BtBase.h"


class AKillerClown;
class AEnemy;
class AStar;
class JAnimationClip;

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
	void ResetBT() override;

public:
	// Action Function
	NodeStatus Attack1();
	NodeStatus Attack2();
	NodeStatus Attack3();
	NodeStatus JumpAttack();
	void       MoveNPCWithJump(float jumpHeight, float duration);
	NodeStatus Hit();
	NodeStatus IsEventAnim();
	NodeStatus ResurrectPhase();


public:
	// BlackBoard BB;
	bool		IsRun = false;
	FVector     mVelocity;
	bool		bBossBattleOn = false;
	bool		bResurrectCondition = false;
	bool		bIsStandUpReady = false;
	AKillerClown* mOwnerEnemy;
	

private:
	// FORCEINLINE bool IsKeyPressed(EKeyCode InKey) const { return mInputKeyboard.IsKeyPressed(InKey); }
	// FORCEINLINE bool IsKeyDown(EKeyCode InKey) const { return mInputKeyboard.IsKeyDown(InKey); }
	// XKeyboardMouse mInputKeyboard;
};

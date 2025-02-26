#pragma once
#include "Core/Input/XKeyboardMouse.h"
#include "BtBase.h"

class AEnemy;
class AStar;

class BT_Butcher : public BtBase,
				   public std::enable_shared_from_this<BT_Butcher>
{
public:
	BT_Butcher(JTextView InName, AActor* InOwner);
	~BT_Butcher() override;

	void Initialize() override;
	void BeginPlay() override;
	void Destroy() override;

	void Tick(float DeltaTime) override;
	void SetupTree();
	void ResetBT(AActor* NewOwner) override;

public:
	// Action Function
	NodeStatus TalkTo();
	NodeStatus conversation(int idx);
	NodeStatus conversation2(int idx);
	// NodeStatus JumpAttack();
	// void       MoveNPCWithJump(float jumpHeight, float duration);
	NodeStatus Hit();
	NodeStatus Dead();
	NodeStatus StateIdleToConvers();
	NodeStatus StateConversToTrace(int N);
	NodeStatus GetNextConvers();
	NodeStatus GetPath(FVector2 GoalGrid);
	NodeStatus GoGoal();
	NodeStatus LookAt(FVector direction);
	NodeStatus StateToNextQuest();
	NodeStatus StateToTransform(int N);
	NodeStatus TransformPhase();

	// Decorator Function
	NodeStatus IsIdle();
	NodeStatus IsConvers();
	NodeStatus IsTrace();
	NodeStatus IsQuestFinished(int n);
	NodeStatus IsQuestEnd();
	NodeStatus IsPressedKey(EKeyCode Key);
	


	// Just Function


private:
	bool    bIsIdle      = true;
	bool    bIsConvers   = false;
	bool    bIsTrace     = false;
	bool    bIsNextQuest = false;
	bool	bIsTransform = false;
	int     conversIdx   = 0;
	FVector npcLocation = FVector::ZeroVector;
	FVector npcRotation = FVector::ZeroVector;
	

	AEnemy*            mOwnerEnemy;

private:
	FORCEINLINE bool IsKeyPressed(EKeyCode InKey) const { return mInputKeyboard.IsKeyPressed(InKey); }
	FORCEINLINE bool IsKeyDown(EKeyCode InKey) const { return mInputKeyboard.IsKeyDown(InKey); }
	FORCEINLINE bool IsKeyUp(EKeyCode InKey) const { return mInputKeyboard.IsKeyUp(InKey); }

	XKeyboardMouse mInputKeyboard;
};

#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "BTBuilder.h"
#include "Core/Utils/Math/Vector.h"
#include "enums.h"
#include "Core/Entity/Navigation/Node.h"

class AStar;

class BtBase : public JActorComponent,
			   public std::enable_shared_from_this<BtBase>
{
public:
	BtBase(JTextView InName, AActor* InOwner);
	~BtBase() override;

	void         Initialize() override;
	void         BeginPlay() override;
	void         Destroy() override;
	void         Tick(float DeltaTime) override;
	virtual void ResetBT(AActor* NewOwner = nullptr);

public:
	// Action Function
	NodeStatus ChasePlayer(const UINT N);
	void       FollowPath();

	// Decorator Function
	NodeStatus IsPlayerClose();
	NodeStatus Not(NodeStatus state);
	NodeStatus RandP(float p);
	NodeStatus RandTime(JText key, float t, float p);
	NodeStatus IsPhase(int phase);
	NodeStatus SyncFrame();


	// Just Function
	FVector RotateTowards(FVector direction, FVector rotation);
	FVector GetPlayerDirection();
	float   GetFloorHeight();
	void    SetYVelocity(float velocity);
	float   GetYVelocity();
	bool    IsPlayerClose(float length);
	bool    IsPlayerLookingAway();

public:
	Ptr<AStar>                       PaStar;
	float                            mDeltaTime;
	FVector                          LastPlayerPos   = FVector(-201, 0, 201);
	bool                             mIsPosUpdated   = false;
	float                            mElapsedTime    = 0;
	bool                             mEventStartFlag = true;
	bool                             NeedsPathReFind = true;
	int                              mPhase          = 1;
	int                              mIdx;
	bool                             mHasPath          = false;
	bool                             runningFlag       = false;
	EFloorType                       mFloorType        = EFloorType::FirstFloor;
	bool                             isPendingKill     = false;
	bool                             bPlayerCloseEvent = false;
	std::unordered_map<JText, float> BB_ElapsedTime;
	int                              mAttackDistance = 300;

protected:
	template <typename TBuilder>
	TBuilder&     AddPhaseSubtree(TBuilder& builder, int phase);
	BTBuilder     builder;
	Ptr<Bt::Node> BTRoot;

	inline static int32_t g_Index = 0;
};

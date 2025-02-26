﻿#pragma once
#include "Core/Entity/Actor/Character/ACharacter.h"

class JAnimator;

enum class EEnemyType : uint8_t
{
	Kihyun,
	Girl,
	Clown,
	Madre,
	Pig,
	Butcher,
	MAX // 이건 숫자 사용
};

enum class EEnemyState : uint8_t
{
	Idle,
	Walk,
	Run,
	Attack,
	Death,
	Convers1,
	Convers2,
	Convers3,
	Convers4,
	ConversSad,
	CrawlRun,
	Sit2StandUp,
	MAX
};

constexpr const char* GetEnemyTypeString(EEnemyType InType)
{
	switch (InType)
	{
	case EEnemyType::Kihyun:
		return "Kihyun";
	case EEnemyType::Girl:
		return "Girl";
	case EEnemyType::Clown:
		return "Clown";
	case EEnemyType::Madre:
		return "Madre";
	case EEnemyType::Pig:
		return "Pig";
	case EEnemyType::Butcher:
		return "Butcher";
	case EEnemyType::MAX:
		break;
	}
	return "Unknown";
}

class AEnemy;
DECLARE_DYNAMIC_DELEGATE(FOnEnemyHit, const FHitResult& HitResult);

DECLARE_DYNAMIC_DELEGATE(FOnEnemyOut);

DECLARE_DYNAMIC_DELEGATE(FOnInteractionStart);

DECLARE_DYNAMIC_DELEGATE(FOnInteractionEnd);

class AEnemy : public APawn
{
public:
	FOnEnemyHit         OnEnemyHit;
	FOnEnemyOut         OnEnemyOut;
	FOnInteractionStart OnInteractionStart;
	FOnInteractionEnd   OnInteractionEnd;

public:
	// Constructor
	AEnemy();
	// Constructor
	AEnemy(JTextView InName);
	// Destructor
	~AEnemy() override = default;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void ShowEditor() override;

public:
	virtual void OnHit(ICollision* InActor, const FHitResult& HitResult);
	virtual void OnOut(ICollision* InActor, const FHitResult& HitResult);

	void        SetEnemyType(EEnemyType InType);
	void        SetEnemyState(EEnemyState InNewState) { mEnemyState = InNewState; }
	EEnemyState GetEnemyState() { return mEnemyState; }

	void EnableAttackCollision(float radius);
	void DisableAttackCollision();

protected:
	EEnemyType                    mEnemyType = EEnemyType::MAX;
	EEnemyState                   mEnemyState;
	UPtr<JAnimator>               mAnimator;
	class JSkeletalMeshComponent* mSkeletalMeshComponent;
	class BtBase*                 mBehaviorTree;

	friend class JKihyunAnimator;
	friend class JGirlAnimator;
	friend class JMadreAnimator;
	friend class JButcherAnimator;
	friend class JPigAnimator;
};

REGISTER_CLASS_TYPE(AEnemy)

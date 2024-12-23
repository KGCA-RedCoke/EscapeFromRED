﻿#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "Core/Utils/Math/Vector.h"

class APawn;

enum class EMovementMode : uint8_t
{
	Walking,
	Running,
	Crouching,
	Falling,
	Swimming,
	Flying,
	Custom
};

class JPawnMovementComponent : public JActorComponent
{
public:
	JPawnMovementComponent(JTextView Name, APawn* OwnerPawn);
	~JPawnMovementComponent() override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

	void ShowEditor() override;

public:
	virtual void Jump();

protected:
	FVector mMoveDirection;		// 이동 방향
	FVector mCachedMoveDirection;	// 캐시된 이동 방향

	FVector mPreviousLocation;	// 이전 위치
	FVector mLocation;	// 소유자 위치

	FVector mMoveVelocity;		// 이동 속도
	FVector mMoveAcceleration;	// 이동 가속도
	FVector mLastMoveVelocity;	// 이전 이동 속도
	FVector mLastMoveAcceleration;	// 이전 이동 가속도

	EMovementMode mMovementMode;

	bool bIsGrounded;			// 지면에 붙어 있는지 여부
	bool bIsJumping;			// 점프 중인지 여부
	bool bIsFalling;			// 낙하 중인지 여부

	float mMaxWalkSpeed;		// 최대 이동 속도
	float mGravityScale;		// 중력 스케일
	float mJumpPower;			// 점프력

	FVector mVelocity;			// 현재 속도
	FVector mAcceleration;		// 가속도
	FVector mAirResistance;		// 공기 저항
	FVector mGroundFriction;	// 지면 마찰력
};

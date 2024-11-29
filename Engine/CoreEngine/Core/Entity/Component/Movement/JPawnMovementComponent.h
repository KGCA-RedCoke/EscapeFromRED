#pragma once
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
	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

protected:
	APawn* mOwnerPawn;

	EMovementMode mMovementMode;

	float mMaxWalkSpeed;		// 최대 이동 속도

	FVector mVelocity;			// 현재 속도
	FVector mAcceleration;		// 가속도
	FVector mAirResistance;		// 공기 저항
	FVector mGroundFriction;	// 지면 마찰력
	FVector mPreviousLocation;	// 이전 위치
};

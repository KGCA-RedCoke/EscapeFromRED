#include "JPawnMovementComponent.h"

#include "Core/Entity/Actor/APawn.h"

JPawnMovementComponent::JPawnMovementComponent(JTextView Name, APawn* OwnerPawn)
	: JActorComponent(Name),
	  mOwnerPawn(OwnerPawn),
	  mMovementMode(),
	  mMaxWalkSpeed(350.f)
{}

JPawnMovementComponent::~JPawnMovementComponent()
{}

void JPawnMovementComponent::Initialize()
{
	JActorComponent::Initialize();
}

void JPawnMovementComponent::BeginPlay()
{
	JActorComponent::BeginPlay();
}

void JPawnMovementComponent::Tick(float DeltaTime)
{
	JActorComponent::Tick(DeltaTime);

	const FVector location    = mOwnerPawn->GetWorldLocation();
	FVector       newVelocity = (location - mPreviousLocation);
	newVelocity /= DeltaTime;

	mAcceleration     = (newVelocity - mVelocity) / DeltaTime;
	mVelocity         = newVelocity;
	mPreviousLocation = location;
}

void JPawnMovementComponent::Destroy()
{
	JActorComponent::Destroy();
}

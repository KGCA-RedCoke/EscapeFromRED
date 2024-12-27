#include "JPawnMovementComponent.h"

#include "Core/Entity/Actor/APawn.h"

JPawnMovementComponent::JPawnMovementComponent(JTextView Name, APawn* OwnerPawn)
	: JActorComponent(Name, OwnerPawn),
	  mMovementMode(),
	  bIsGrounded(false),
	  bIsJumping(false),
	  bIsFalling(false),
	  mMaxWalkSpeed(350.f),
	  mGravityScale(0),
	  mJumpPower(0)
{}

JPawnMovementComponent::~JPawnMovementComponent() = default;

uint32_t JPawnMovementComponent::GetType() const
{
	return HASH_COMPONENT_TYPE_PawnMovement;
}

bool JPawnMovementComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JActorComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	// Walk Speed
	Utils::Serialization::Serialize_Primitive(&mMaxWalkSpeed, sizeof(float), FileStream);

	// Gravity Scale
	Utils::Serialization::Serialize_Primitive(&mGravityScale, sizeof(float), FileStream);

	// Jump Power
	Utils::Serialization::Serialize_Primitive(&mJumpPower, sizeof(float), FileStream);

	return true;
}

bool JPawnMovementComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JActorComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	// Walk Speed
	Utils::Serialization::DeSerialize_Primitive(&mMaxWalkSpeed, sizeof(float), InFileStream);

	// Gravity Scale
	Utils::Serialization::DeSerialize_Primitive(&mGravityScale, sizeof(float), InFileStream);

	// Jump Power
	Utils::Serialization::DeSerialize_Primitive(&mJumpPower, sizeof(float), InFileStream);

	return true;
}

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

	const FVector thisFrameLocation = mOwnerActor->GetWorldLocation();
	FVector       newVelocity       = (thisFrameLocation - mPreviousLocation);
	newVelocity /= DeltaTime;

	mAcceleration     = (newVelocity - mVelocity) / DeltaTime;
	mVelocity         = newVelocity;
	mCurrentWalkSpeed = mVelocity.Length();
	mPreviousLocation = thisFrameLocation;
}

void JPawnMovementComponent::Destroy()
{
	JActorComponent::Destroy();
}

void JPawnMovementComponent::ShowEditor()
{
	// Max Walk Speed
	ImGui::InputFloat("Max Walk Speed", &mMaxWalkSpeed);

	ImGui::Text("Current Walk Speed: %f", mCurrentWalkSpeed);
	ImGui::Text("Current Velocity: %f, %f, %f", mVelocity.x, mVelocity.y, mVelocity.z);
	ImGui::Text("Current Acceleration: %f, %f, %f", mAcceleration.x, mAcceleration.y, mAcceleration.z);
}

void JPawnMovementComponent::Jump()
{
	if (!bIsGrounded || bIsJumping)
	{
		return;
	}

	mVelocity.y = mJumpPower;
	bIsJumping  = true;
	bIsGrounded = false;
}

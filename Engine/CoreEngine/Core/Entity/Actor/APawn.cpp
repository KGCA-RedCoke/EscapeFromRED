#include "APawn.h"

#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"

APawn::APawn(JTextView Name) {}

APawn::~APawn() {}

void APawn::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
}

void APawn::Destroy()
{
	AActor::Destroy();
}

void APawn::Initialize()
{
	AActor::Initialize();

	mMovementComponent = CreateDefaultSubObject<JPawnMovementComponent>("MovementComponent", this);
}

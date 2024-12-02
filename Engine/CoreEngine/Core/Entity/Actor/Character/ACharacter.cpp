#include "ACharacter.h"

#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Input/XKeyboardMouse.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Player/JPlayerCamera.h"

ACharacter::ACharacter() {}

ACharacter::ACharacter(JTextView InName)
	: AActor(InName),
	  mSkeletalMeshComponent(nullptr),
	  mFPSCamera(nullptr) {}

void ACharacter::Initialize()
{
	AActor::Initialize();
}

void ACharacter::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	mLocalRotation.y = XMConvertToDegrees(mFPSCamera->GetYaw());
}

void ACharacter::Destroy()
{
	AActor::Destroy();
}

FVector2 ACharacter::GetMouseDelta() const
{
	return mInput->GetCurMouseDelta();
}

#include "APlayerCharacter.h"
#include "JPlayerCamera.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Interface/JWorld.h"

APlayerCharacter::APlayerCharacter(JTextView InName, JTextView InMeshPath)
	: ACharacter(InName)
{
	mSkeletalMeshComponent = CreateDefaultSubObject<
		JSkeletalMeshComponent>(ParseFile(InMeshPath.data()), this, this);
	mSkeletalMeshComponent->SetSkeletalMesh(InMeshPath);
	mSkeletalMeshComponent->SetLocalRotation({0, 0, 0});

	mBoundingBox = mSkeletalMeshComponent->GetBoundingVolume();

	mFPSCamera = CreateDefaultSubObject<JPlayerCamera>("FPSCamera", this, mSkeletalMeshComponent);
	GetWorld.CameraManager->SetCurrentMainCam(mFPSCamera);
}

void APlayerCharacter::Initialize()
{
	ACharacter::Initialize();

	SetupInputComponent();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	mInput->Update(DeltaTime);

	ACharacter::Tick(DeltaTime);
}

void APlayerCharacter::Destroy()
{
	ACharacter::Destroy();
}

void APlayerCharacter::SetupInputComponent()
{
	mInput = MakeUPtr<XKeyboardMouse>();
	mInput->Initialize();


	mInput->AddInputBinding(EKeyCode::A,
							EKeyState::Pressed,
							std::bind(&APlayerCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  -FVector::RightVector));
	mInput->AddInputBinding(EKeyCode::D,
							EKeyState::Pressed,
							std::bind(&APlayerCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  FVector::RightVector));
	mInput->AddInputBinding(EKeyCode::W,
							EKeyState::Pressed,
							std::bind(&APlayerCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  FVector::ForwardVector));
	mInput->AddInputBinding(EKeyCode::S,
							EKeyState::Pressed,
							std::bind(&APlayerCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  -FVector::ForwardVector));
}

void APlayerCharacter::OnMovementInputPressed(float DeltaTime, const FVector& InDirection)
{
	FVector directionVec = FVector::ZeroVector;

	if (InDirection.z != 0)
	{
		directionVec = InDirection.z < 0
						   ? -mFPSCamera->GetFlatForwardVector()
						   : +mFPSCamera->GetFlatForwardVector();
	}
	if (InDirection.x != 0)
	{
		directionVec = InDirection.x < 0
						   ? -mFPSCamera->GetFlatRightVector()
						   : +mFPSCamera->GetFlatRightVector();
	}
	mLocalLocation += directionVec * 500.0f * DeltaTime;
}

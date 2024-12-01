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

ASampleCharacter::ASampleCharacter()
{}

ASampleCharacter::ASampleCharacter(JTextView InName, JTextView InMeshPath)
	: ACharacter(InName)
{
	mSkeletalMeshComponent = CreateDefaultSubObject<
		JSkeletalMeshComponent>(ParseFile(InMeshPath.data()), this, this);
	mSkeletalMeshComponent->SetSkeletalMesh(InMeshPath);
	mSkeletalMeshComponent->SetLocalRotation({0, 180.f, 0});

	mFPSCamera = CreateDefaultSubObject<JPlayerCamera>("FPSCamera", this, mSkeletalMeshComponent);
	GetWorld.CameraManager->SetCurrentMainCam(mFPSCamera);
}

void ASampleCharacter::Initialize()
{
	ACharacter::Initialize();

	SetupInputComponent();
}

void ASampleCharacter::Tick(float DeltaTime)
{
	mInput->Update(DeltaTime);

	ACharacter::Tick(DeltaTime);
}

void ASampleCharacter::Destroy()
{
	ACharacter::Destroy();
}

void ASampleCharacter::SetupInputComponent()
{
	mInput = MakeUPtr<XKeyboardMouse>();
	mInput->Initialize();


	mInput->AddInputBinding(EKeyCode::A,
							EKeyState::Pressed,
							std::bind(&ASampleCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  -FVector::RightVector));
	mInput->AddInputBinding(EKeyCode::D,
							EKeyState::Pressed,
							std::bind(&ASampleCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  FVector::RightVector));
	mInput->AddInputBinding(EKeyCode::W,
							EKeyState::Pressed,
							std::bind(&ASampleCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  FVector::ForwardVector));
	mInput->AddInputBinding(EKeyCode::S,
							EKeyState::Pressed,
							std::bind(&ASampleCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  -FVector::ForwardVector));

}

void ASampleCharacter::OnMovementInputPressed(float DeltaTime, const FVector& InDirection)
{
	FVector directionVec = FVector::ZeroVector;

	if (InDirection.z != 0)
	{
		directionVec = InDirection.z > 0
						   ?  - mFPSCamera->GetFlatForwardVector()
						   :  + mFPSCamera->GetFlatForwardVector();
	}
	if (InDirection.x != 0)
	{
		directionVec = InDirection.x > 0
						   ?  - mFPSCamera->GetFlatRightVector()
						   :  + mFPSCamera->GetFlatRightVector();
	}
	mLocalLocation += directionVec * 500.0f * DeltaTime;
}

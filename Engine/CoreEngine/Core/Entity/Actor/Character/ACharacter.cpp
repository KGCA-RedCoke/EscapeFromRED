#include "ACharacter.h"

#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Input/XKeyboardMouse.h"
#include "Core/Interface/JWorld.h"

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
}

void ACharacter::Destroy()
{
	AActor::Destroy();
}

ASampleCharacter::ASampleCharacter()
{}

ASampleCharacter::ASampleCharacter(JTextView InName, JTextView InMeshPath)
	: ACharacter(InName)
{
	mSkeletalMeshComponent = CreateDefaultSubObject<JSkeletalMeshComponent>(ParseFile(InMeshPath.data()), this, this);
	mSkeletalMeshComponent->SetSkeletalMesh(InMeshPath);

	mFPSCamera = CreateDefaultSubObject<JCameraComponent>("FPSCamera", this, this);
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
									  FVector2(-1.0f, 0.0f)));
	mInput->AddInputBinding(EKeyCode::D,
							EKeyState::Pressed,
							std::bind(&ASampleCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  FVector2(1.0f, 0.0f)));
	mInput->AddInputBinding(EKeyCode::W,
							EKeyState::Pressed,
							std::bind(&ASampleCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  FVector2(0.0f, 1.0f)));
	mInput->AddInputBinding(EKeyCode::S,
							EKeyState::Pressed,
							std::bind(&ASampleCharacter::OnMovementInputPressed,
									  this,
									  std::placeholders::_1,
									  FVector2(0.0f, -1.0f)));
}

void ASampleCharacter::OnMovementInputPressed(float DeltaTime, const FVector2& InDirection)
{
	mLocalLocation.z += InDirection.y * 100.0f * DeltaTime;
	mLocalLocation.x += InDirection.x * 100.0f * DeltaTime;
}

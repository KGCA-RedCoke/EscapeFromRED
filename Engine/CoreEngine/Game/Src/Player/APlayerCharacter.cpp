#include "APlayerCharacter.h"
#include "JPlayerCamera.h"
#include "Core/Entity/Animation/JAnimationClip.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Entity/Light/JLight_Point.h"
#include "Core/Interface/JWorld.h"

APlayerCharacter::APlayerCharacter()
	: ACharacter()
{
	mObjectType = NAME_OBJECT_PLAYER_CHARACTER;
	APlayerCharacter::Initialize();
}

APlayerCharacter::APlayerCharacter(JTextView InName, JTextView InMeshPath)
	: ACharacter(InName)
{
	mObjectType = NAME_OBJECT_PLAYER_CHARACTER;
	SetFlag(EObjectFlags::IgnoreFrustum);

	APlayerCharacter::SetupInputComponent();

	mSkeletalMeshComponent = CreateDefaultSubObject<
		JSkeletalMeshComponent>(ParseFile(InMeshPath.data()), this, this);
	mSkeletalMeshComponent->SetLocalRotation({0, -180, 0});
	mSkeletalMeshComponent->SetSkeletalMesh(InMeshPath);

	auto* light = CreateDefaultSubObject<
		JLight_Point>("Light", this, this);
	light->SetupAttachment(mSkeletalMeshComponent);
	light->SetLocalLocation({-23.000, 146, -43});
	light->SetLocalRotation({0, -2.5, 0});

	mWalkAnimation = MakeUPtr<JAnimationClip>();
	if (!Utils::Serialization::DeSerialize("Game/Animation/Anim_Hands__Lantern_01_Walk.jasset", mWalkAnimation.get()))
	{
		LOG_CORE_ERROR("Failed to load animation object(Invalid Path maybe...): {0}",
					   "Game/Animation/Anim_Hands_Empty_Walk.jasset");
	}

	mSkeletalMeshComponent->SetAnimation(mWalkAnimation.get());

	mBoundingBox = mSkeletalMeshComponent->GetBoundingVolume();

	mFPSCamera = CreateDefaultSubObject<JPlayerCamera>("FPSCamera", this, mSkeletalMeshComponent);
	GetWorld.CameraManager->SetCurrentMainCam(mFPSCamera);
}

void APlayerCharacter::Initialize()
{
	ACharacter::Initialize();

	SetupInputComponent();

	mSkeletalMeshComponent = CreateDefaultSubObject<
		JSkeletalMeshComponent>("SkeletalMeshComp", this, this);

	mFPSCamera = CreateDefaultSubObject<JPlayerCamera>("FPSCamera", this, mSkeletalMeshComponent);
	GetWorld.CameraManager->SetCurrentMainCam(mFPSCamera);
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

uint32_t APlayerCharacter::GetType() const
{
	return HASH_ASSET_TYPE_PLAYER_CHARACTER;
}

bool APlayerCharacter::Serialize_Implement(std::ofstream& FileStream)
{
	return ACharacter::Serialize_Implement(FileStream);
}

bool APlayerCharacter::DeSerialize_Implement(std::ifstream& InFileStream)
{
	return ACharacter::DeSerialize_Implement(InFileStream);
}

void APlayerCharacter::ShowEditor()
{
	ACharacter::ShowEditor();


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

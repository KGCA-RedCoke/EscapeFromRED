#include "APlayerCharacter.h"

#include <DirectXColors.h>

#include "JPlayerAnimator.h"
#include "JPlayerCamera.h"
#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Interface/JWorld.h"

APlayerCharacter::APlayerCharacter()
	: ACharacter()
{}

APlayerCharacter::APlayerCharacter(JTextView InName, JTextView InMeshPath)
	: ACharacter(InName)
{
	// 프러스텀 무시
	// SetFlag(IgnoreFrustum);

	// 스켈레탈 부착
	mSkeletalMeshComponent = CreateDefaultSubObject<
		JSkeletalMeshComponent>(ParseFile(InMeshPath.data()), this, this);
	mSkeletalMeshComponent->SetSkeletalMesh(InMeshPath);
	mSkeletalMeshComponent->SetLocalRotation({0, 180, 0});
	mPlayerAnimator = MakeUPtr<JPlayerAnimator>(InName, mSkeletalMeshComponent);
	mSkeletalMeshComponent->SetAnimator(mPlayerAnimator.get());

	// 무기 부착
	mWeaponMesh = CreateDefaultSubObject<JStaticMeshComponent>("Weapon", this);
	mWeaponMesh->SetupAttachment(mSkeletalMeshComponent);
	mWeaponMesh->SetMeshObject("Game/Mesh/SM_Pickaxe_.jasset");
	mWeaponMesh->AttachToBoneSocket(mSkeletalMeshComponent, "hand_r");
	mWeaponMesh->SetLocalLocation({-53, 110, 54});
	mWeaponMesh->SetLocalRotation({-127, 89, -82});

	// 무기 콜라이더 부착
	mWeaponCollision = CreateDefaultSubObject<JBoxComponent>("WeaponCollision", this);
	mWeaponCollision->SetTraceType(ETraceType::PlayerWeapon);
	mWeaponCollision->SetupAttachment(mWeaponMesh);
	mWeaponCollision->SetLocalLocation({0, 100, 0});
	mWeaponCollision->SetLocalScale({0.5, 0.5, 0.5f});
	mWeaponCollision->SetColor(DirectX::Colors::Orange);
	mWeaponCollision->EnableCollision(false);

	mBoundingBox = mSkeletalMeshComponent->GetBoundingVolume();

	mFPSCamera = CreateDefaultSubObject<JPlayerCamera>("FPSCamera", this);
	mFPSCamera->SetupAttachment(mSkeletalMeshComponent);

	GetWorld.CameraManager->SetCurrentMainCam(mFPSCamera);
}

void APlayerCharacter::Initialize()
{
	ACharacter::Initialize();
	mPlayerAnimator->Initialize();
}

void APlayerCharacter::BeginPlay()
{
	ACharacter::BeginPlay();
	SetupInputComponent();
	SetLocalLocation({0, 150, 0});
}

void APlayerCharacter::Tick(float DeltaTime)
{
	bMove = false;

	if (mInput)
	{
		mInput->Update(DeltaTime);
	}
	UpdateRotation();
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

	mInput->AddInputBinding(EKeyCode::LButton,
							EKeyState::Pressed,
							[this](float DeltaTime){ bShouldAttack = true; });
}

void APlayerCharacter::UpdateRotation()
{
	mMouseDelta.x  = 0.f;
	mRotVelocity.x = 0.f;

	float fPercentOfNew = 1.0f / 2.f;
	float fPercentOfOld = 1.0f - fPercentOfNew;

	mMouseDelta.x = mMouseDelta.x * fPercentOfOld + mInput->GetCurMouseDelta().x * fPercentOfNew;
	mMouseDelta.y = mMouseDelta.y * fPercentOfOld + mInput->GetCurMouseDelta().y * fPercentOfNew;

	mRotVelocity.x = mMouseDelta.x * (XM_PI / 360.0);
	mRotVelocity.y = mMouseDelta.y * (XM_PI / 360.0);

	float yawDelta   = mRotVelocity.x;
	float pitchDelta = mRotVelocity.y;

	mYaw += yawDelta;
	mPitch += pitchDelta;

	mPitch = std::clamp(mPitch, -XM_PIDIV2, XM_PIDIV2);

	mLocalRotation.y = XMConvertToDegrees(mYaw);
}

void APlayerCharacter::OnMovementInputPressed(float DeltaTime, const FVector& InDirection)
{
	FVector directionVec = FVector::ZeroVector;
	bMove                = true;
	if (InDirection.z != 0)
	{
		directionVec = directionVec + (InDirection.z < 0.f
										   ? -mFPSCamera->GetFlatForwardVector()
										   : +mFPSCamera->GetFlatForwardVector());
	}
	if (InDirection.x != 0)
	{
		directionVec = directionVec + (InDirection.x < 0
										   ? -mFPSCamera->GetFlatRightVector()
										   : +mFPSCamera->GetFlatRightVector());
	}

	// 방향 벡터를 정규화한 뒤 이동량을 곱하기
	directionVec.Normalize();

	// 현재 위치 업데이트
	AddLocalLocation(directionVec * 500.0f * DeltaTime);
}

void APlayerCharacter::CheckGround()
{}

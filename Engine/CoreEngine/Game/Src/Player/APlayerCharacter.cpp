#include "APlayerCharacter.h"

#include <DirectXColors.h>

#include "JPlayerAnimator.h"
#include "JPlayerCamera.h"
#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Entity/UI/MUIManager.h"
#include "Core/Interface/JWorld.h"

APlayerCharacter::APlayerCharacter()
	: ACharacter()
{}

APlayerCharacter::APlayerCharacter(JTextView InName, JTextView InMeshPath)
	: ACharacter(InName)
{
	// 프러스텀 무시
	SetFlag(IgnoreFrustum);

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
	mWeaponCollision = CreateDefaultSubObject<JSphereComponent>("WeaponCollision", this);
	mWeaponCollision->SetTraceType(ETraceType::PlayerWeapon);
	mWeaponCollision->SetupAttachment(mWeaponMesh);
	mWeaponCollision->SetLocalLocation({0, 100, 0});
	mWeaponCollision->SetLocalScale({0.5, 0.5, 0.5f});
	mWeaponCollision->SetColor(DirectX::Colors::Orange);
	mWeaponCollision->EnableCollision(false);

	mBoundingBox = mSkeletalMeshComponent->GetBoundingVolume();

	mFPSCamera = CreateDefaultSubObject<JPlayerCamera>("FPSCamera", this);
	mFPSCamera->SetupAttachment(mSkeletalMeshComponent);

	OnPlayerHealthChanged.Bind([this](bool bIncrease){
		if (bIncrease)
		{
			// 체력 회복
			mHealth = FMath::Clamp(mHealth + 1, 1u, mMaxHealth);

			GetWorld.LevelManager->GetActiveLevel()->IncreaseHPBar(mHealth);
		}
		else
		{
			// 체력 감소
			if (mHealth <= 1)
			{
				OnPlayerDie.Execute();
				return;
			}
			mHealth--;

			GetWorld.LevelManager->GetActiveLevel()->DecreamentHPBar(mHealth);

		}
	});

	OnPlayerDie.Bind([this](){
		// 플레이어 사망
		// 게임 오버
		GetWorld.LevelManager->GetActiveLevel()->mGameOverWidget->SetVisible(true);
	});


	GetWorld.CameraManager->SetCurrentMainCam(mFPSCamera);
}

void APlayerCharacter::Initialize()
{
	ACharacter::Initialize();
	mCollisionSphere->OnComponentBeginOverlap.Bind([this](ICollision* OtherActor, const FHitResult& HitResult){
		if (OtherActor->GetTraceType() == ETraceType::EnemyHitSpace)
		{
			OnPlayerHealthChanged.Execute(false);
		}
	});
	mPlayerAnimator->Initialize();
}

void APlayerCharacter::BeginPlay()
{
	ACharacter::BeginPlay();
	SetupInputComponent();


	for (int32_t i = 0; i < mHealth; ++i)
	{
		GetWorld.LevelManager->GetActiveLevel()->SetHPBar(i, true);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	if (mInput && !bLockInput)
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

	mInput->AddInputBinding(EKeyCode::LShift,
							EKeyState::Pressed,
							[this](float DeltaTime){ bShouldRun = true; });
	mInput->AddInputBinding(EKeyCode::LShift,
							EKeyState::Up,
							[this](float DeltaTime){ bShouldRun = false; });

	mInput->AddInputBinding(EKeyCode::LButton,
							EKeyState::Pressed,
							[this](float DeltaTime){
								bShouldAttack = true;
							});
}

void APlayerCharacter::UpdateRotation()
{
	FVector2 currentMousePosition = mInput->GetMousePosition();
	FVector2 screenCenter         = GetWorld.ScreenSize / 2;
	float    halfWidth            = GetWorld.ScreenSize.x / 2.0f;
	float    halfHeight           = GetWorld.ScreenSize.y / 2.0f;
	float    offsetX              = (currentMousePosition.x - screenCenter.x) / halfWidth;
	float    threshold            = 0.9f;
	// 화면 중심 기준으로 Yaw와 Pitch 추가
	if (fabs(offsetX) > threshold)
	{
		SetCursorPos(screenCenter.x, currentMousePosition.y);
		return;
	}

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
	AddLocalLocation(directionVec * (bShouldRun
										 ? mMovementComponent->GetMaxJogSpeed()
										 : mMovementComponent->GetMaxWalkSpeed()) * DeltaTime
					);
}

void APlayerCharacter::OnMeleeAttack()
{
	mWeaponCollision->EnableCollision(true);
	bShouldAttack = false;
}

void APlayerCharacter::DisableMeleeCollision()
{
	mWeaponCollision->EnableCollision(false);
}

void APlayerCharacter::OnMeleeAttackFinished()
{

	switch (mAttackCombo)
	{
	case 0:
		mAttackCombo = bShouldAttack ? 1 : 0;
		break;
	case 1:
		mAttackCombo = bShouldAttack ? 2 : 0;
		break;
	case 2:
		mAttackCombo = bShouldAttack ? 3 : 0;
		break;
	default:
		mAttackCombo = 0;
		break;
	}
	bShouldAttack = false;
}

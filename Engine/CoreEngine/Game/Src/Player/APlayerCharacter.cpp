#include "APlayerCharacter.h"
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
	SetFlag(IgnoreFrustum);
	mSkeletalMeshComponent = CreateDefaultSubObject<
		JSkeletalMeshComponent>(ParseFile(InMeshPath.data()), this, this);
	mSkeletalMeshComponent->SetSkeletalMesh(InMeshPath);
	mSkeletalMeshComponent->SetLocalRotation({0, 180, 0});
	mAnimList.push_back(GetWorld.AnimationManager->Load("Game/Animation/Anim_Hands__Lantern_01_Run.jasset",
														mSkeletalMeshComponent));
	mAnimList.push_back(GetWorld.AnimationManager->Load("Game/Animation/Anim_Hands__Lantern_01_Walk.jasset",
														mSkeletalMeshComponent));
	mCurrentAnimIndex = 1;
	mSkeletalMeshComponent->SetAnimation(mAnimList.back());

	mLightMesh = CreateDefaultSubObject<JStaticMeshComponent>("LightMesh", this);
	mLightMesh->SetupAttachment(mSkeletalMeshComponent);
	mLightMesh->SetMeshObject("Game/Mesh/SM_Flashlight_01.jasset");
	mLightMesh->AttachToBoneSocket(mSkeletalMeshComponent, "hand_r");
	mLightMesh->SetLocalLocation({-24.950527, -10.713480, -8.060391});
	mLightMesh->SetLocalRotation({-38.99271, 83.490776, -2.771158});

	mBoundingBox = mSkeletalMeshComponent->GetBoundingVolume();

	mFPSCamera = CreateDefaultSubObject<JPlayerCamera>("FPSCamera", this);
	mFPSCamera->SetupAttachment(mSkeletalMeshComponent);

	GetWorld.CameraManager->SetCurrentMainCam(mFPSCamera);

}

void APlayerCharacter::Initialize()
{
	ACharacter::Initialize();

	SetupInputComponent();
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

	if (bMove && mCurrentAnimIndex != 0)
	{
		mSkeletalMeshComponent->SetAnimation(mAnimList[0]);
		mCurrentAnimIndex = 0;
	}
	if (!bMove && mCurrentAnimIndex != 1)
	{
		mSkeletalMeshComponent->SetAnimation(mAnimList[1]);
		mCurrentAnimIndex = 1;
	}
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
	mLocalLocation += directionVec * 500.0f * DeltaTime;
}

void APlayerCharacter::CheckGround()
{}

#include "JPlayerCamera.h"
#include "Core/Entity/Actor/Character/ACharacter.h"

JPlayerCamera::JPlayerCamera() {}

JPlayerCamera::JPlayerCamera(JTextView InName, ACharacter* InOwner, JSceneComponent* InParent)
	: JCameraComponent(InName, InOwner, InParent),
	  mOwnerCharacter(InOwner)
{
	mNearPlane     = 1.f;
	mFOV           = M_PI / 4.f;
	mLocalLocation = FVector{0, 161.6f, -9.8f};
}

void JPlayerCamera::Tick(float DeltaTime)
{
	JSceneComponent::Tick(DeltaTime);

	UpdateRotation(DeltaTime);
	UpdateVelocity(DeltaTime);

	XMMATRIX mCameraRot = XMMatrixRotationRollPitchYaw(mOwnerCharacter->GetPitch(), mOwnerCharacter->GetYaw(), 0);
	mCameraUp           = XMVector3TransformCoord(M_UpVector, mCameraRot);
	mCameraAhead        = XMVector3TransformCoord(M_ForwardVector, mCameraRot);
	mCameraRight        = XMVector3TransformCoord(M_RightVector, mCameraRot);

	XMVECTOR vLookAt = mWorldLocation + mCameraAhead;
	XMStoreFloat3(&mLookAt, vLookAt);

	// Update the view matrix
	XMMATRIX view = XMMatrixLookAtLH(mWorldLocation, vLookAt, mCameraUp);
	XMStoreFloat4x4(&mView, view);

	CreateFrustum();
}

void JPlayerCamera::UpdateRotation(float DeltaTime)
{
	// JCameraComponent::UpdateRotation(DeltaTime);
	//
	// float fPercentOfNew = 1.0f / 2.f;
	// float fPercentOfOld = 1.0f - fPercentOfNew;
	// mMouseDelta.x       = mMouseDelta.x * fPercentOfOld + mOwnerCharacter->GetMouseDelta().x * fPercentOfNew;
	// mMouseDelta.y       = mMouseDelta.y * fPercentOfOld + mOwnerCharacter->GetMouseDelta().y * fPercentOfNew;
	//
	// mRotVelocity.x = mMouseDelta.x * mRotationValue;
	// mRotVelocity.y = mMouseDelta.y * mRotationValue;

}

void JPlayerCamera::UpdateVelocity(float DeltaTime)
{
	JCameraComponent::UpdateVelocity(DeltaTime);


	// mCamSpeed = FMath::Clamp(mCamSpeed + DeltaTime * mCamSpeed, 1.f, 10.f);


	mVelocity *= mCamSpeed;
}

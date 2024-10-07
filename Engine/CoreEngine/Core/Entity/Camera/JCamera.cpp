#include "JCamera.h"

#include "Core/Graphics/ShaderStructs.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Graphics/DXUtils.h"
#include "Core/Window/Application.h"

uint32_t JCamera::s_CameraNum = 0;

/**
 * 이름 지정이 없는 생성자의 경우 카메라 이름을 JCam_0, JCam_1, JCam_2... 순으로 지정.
 */
JCamera::JCamera() noexcept
	: mName(std::format(L"JCam_{}", s_CameraNum++)),
	  mDefaultEye(0, 5.f, -15.f),
	  mDefaultLookAt(0, 0, 0),
	  mEye(mDefaultEye),
	  mLookAt(0, 0, 0),
	  mYaw(0.f),
	  mPitch(0.f),
	  mFOV(0),
	  mAspect(0),
	  mNearPlane(0.1f),
	  mFarPlane(10000.f),
	  mRotationValue(0.01f),
	  mTranslationValue(5.f),
	  mInputKeyboard()
{
	JCamera::SetViewParams(mDefaultEye, mDefaultLookAt);

	const float aspect =
			static_cast<float>(Application::s_AppInstance->GetWindowWidth()) /
			static_cast<float>(Application::s_AppInstance->GetWindowHeight());

	JCamera::SetProjParams(M_PI / 4, aspect, mNearPlane, mFarPlane);

	JCamera::Initialize();
}

JCamera::JCamera(const JText& InName)
	: JCamera()
{
	mName = String2WString(InName);
}

JCamera::JCamera(const JWText& InName)
	: JCamera()
{
	mName = InName;
}

void JCamera::Initialize()
{
	mInputKeyboard.Initialize();

	
	Utils::DX::CreateBuffer(IManager.RenderManager->GetDevice(),
							D3D11_BIND_CONSTANT_BUFFER,
							nullptr,
							sizeof(CBuffer::Camera),
							1,
							mCameraConstantBuffer.GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE);
}

void JCamera::Update(float_t DeltaTime)
{
	mInputKeyboard.Update();
	UpdateInput();
	UpdateRotation(DeltaTime);
	UpdateVelocity(DeltaTime);

	float yawDelta   = mRotVelocity.x;
	float pitchDelta = mRotVelocity.y;

	mYaw += yawDelta;
	mPitch += pitchDelta;

	// Pitch(위 아래)를 제한
	mPitch = max(-XM_PI / 2.0f, mPitch);
	mPitch = min(+XM_PI / 2.0f, mPitch);

	XMVECTOR velocity   = XMLoadFloat3(&mVelocity);
	XMVECTOR posDelta   = velocity * DeltaTime;
	XMMATRIX mCameraRot = XMMatrixRotationRollPitchYaw(mPitch, mYaw, 0);

	// World 기저 벡터를 CameraRotation만큼 회전
	XMVECTOR worldUp       = XMVector3TransformCoord(M_UpVector, mCameraRot);
	XMVECTOR worldAhead    = XMVector3TransformCoord(M_ForwardVector, mCameraRot);
	XMVECTOR posDeltaWorld = XMVector3TransformCoord(posDelta, mCameraRot);


	XMVECTOR vEye = XMLoadFloat3(&mEye);
	vEye += posDeltaWorld;
	XMStoreFloat3(&mEye, vEye);

	XMVECTOR vLookAt = vEye + worldAhead;
	XMStoreFloat3(&mLookAt, vLookAt);

	// Update the view matrix
	XMMATRIX view = XMMatrixLookAtLH(vEye, vLookAt, worldUp);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX mTrans = XMMatrixTranslation(0, 0, mVelocity.z * DeltaTime);
	XMStoreFloat4x4(&mWorld, mTrans);

	// Update the camera constant buffer
	CBuffer::Camera camPos;
	camPos.CameraPos = FVector4(mEye, 1.f);
	Utils::DX::UpdateDynamicBuffer(IManager.RenderManager->GetImmediateDeviceContext(),
								   mCameraConstantBuffer.Get(),
								   &mEye,
								   sizeof(CBuffer::Camera));
}

void JCamera::Release()
{}

uint32_t JCamera::GetHash() const
{
	return StringHash(ParseFile(mName.data()).c_str());
}

void JCamera::Reset()
{
	XMStoreFloat4x4(&mWorld, XMMatrixIdentity());

	SetViewParams(mDefaultEye, mDefaultLookAt);
}

// 매개변수로 XMVECTOR가 아닌 FXMVECTOR를 사용하는 이유는
// 매개변수로 받을 때는 FXMVECTOR를 사용하고, 내부에서는 XMVECTOR를 사용하는 것이 좋다고 한다..
// 너무 많은 데이터를 레지스터에 넣을 수 있기때문(성능 저하) 
void JCamera::SetViewParams(FXMVECTOR InEyeVec, FXMVECTOR InLookAtVec)
{
	/// 카메라가 사용하는 공간(뷰 공간)을 정의해보자.
	/// View Space의 원점은 카메라 렌즈의 정 중앙이고 이를 기준으로 3개의 축을 만들 수 있다.
	/// 이 축을 어떻게 만들 수 있을까?
	/// 카메라의 위치(eye)와 바라보는 방향(lookAt)을 알면 된다.
	/// 그 이유는 위치(eye)에서 바라보는 방향(lookAt)으로 z축을 만들고,
	/// y축은 위 방향(Up 보통은 (0,1,0))으로 만들면
	/// 외적(Cross Product)를 이용해 x축을 만들 수 있다.

	XMStoreFloat3(&mDefaultEye, InEyeVec);
	XMStoreFloat3(&mEye, InEyeVec);

	XMStoreFloat3(&mDefaultLookAt, InLookAtVec);
	XMStoreFloat3(&mLookAt, InLookAtVec);

	/// 카메라 뷰행렬 계산
	/// UP Vector는 보통 (0,1,0)을 사용한다.
	/// 왜냐? Roll(회전)을 하지 않는다면 카메라의 Up Vector는 항상 (0,1,0)에서 바뀔 이유가 없다.
	/// Roll회전을 할거면 Up Vector를 바꿔야 한다.
	XMMATRIX viewMat = XMMatrixLookAtLH(InEyeVec, InLookAtVec, M_UpVector);
	XMStoreFloat4x4(&mView, viewMat);

	// 역행렬 (의미상, 카메라 -> 월드행렬로 변환하는 행렬) 그러면 카메라의 월드 좌표계에서의 위치와 방향을 알 수 있다.
	XMMATRIX inverseViewMat = XMMatrixInverse(nullptr, viewMat);

	// z기저 벡터(카메라의 전면벡터)를 추출 
	XMFLOAT3 zBasis;
	XMStoreFloat3(&zBasis, inverseViewMat.r[2]);

	// 카메라 Forward, Right Vector가 이루는 성분으로 길이 계산 (X, Z 평면 상에서 각도 계산)
	mYaw = atan2f(zBasis.x, zBasis.z);

	float len = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
	// Up Vector와 아래 면벡터(X, Z)가 이루는 각도 계산
	mPitch = -asinf(zBasis.y / len);
}

void JCamera::SetProjParams(float InFOV, float InAspect, float InNearPlane, float InFarPlane)
{
	mFOV       = InFOV;
	mAspect    = InAspect;
	mNearPlane = InNearPlane;
	mFarPlane  = InFarPlane;

	XMMATRIX projMat = XMMatrixPerspectiveFovLH(InFOV, InAspect, InNearPlane, InFarPlane);
	XMStoreFloat4x4(&mProj, projMat);
}

void JCamera::SetCameraConstantBuffer(uint32_t InSlot)
{
	IManager.RenderManager->GetImmediateDeviceContext()->VSSetConstantBuffers(InSlot, 1, mCameraConstantBuffer.GetAddressOf());
}

void JCamera::UpdateVelocity(float DeltaTime)
{
	XMVECTOR vMouseDelta  = XMLoadFloat2(&mMouseDelta);
	XMVECTOR vRotVelocity = vMouseDelta * mRotationValue;

	XMStoreFloat2(&mRotVelocity, vRotVelocity);

	XMVECTOR keyboardDirection = XMLoadFloat3(&mInputDirection);
	XMVECTOR acceleration      = keyboardDirection;

	acceleration = XMVector3Normalize(acceleration);

	acceleration *= mTranslationValue;

	if (bMovementDrag)
	{
		if (XMVectorGetX(XMVector3LengthSq(acceleration)) > 0)
		{
			XMStoreFloat3(&mVelocity, acceleration);

			mDragTimer = mTotalDragTimeToZero;

			XMStoreFloat3(&mDragVelocity, acceleration / mDragTimer);
		}
		else
		{
			if (mDragTimer > 0)
			{
				// Drag until timer is <= 0
				XMVECTOR vVelocity     = XMLoadFloat3(&mVelocity);
				XMVECTOR vVelocityDrag = XMLoadFloat3(&mDragVelocity);

				vVelocity -= vVelocityDrag * DeltaTime;

				XMStoreFloat3(&mVelocity, vVelocity);

				mDragTimer -= DeltaTime;
			}
			else
			{
				// Zero velocity
				mVelocity = FVector::ZeroVector;
			}
		}
	}
	else
	{
		XMStoreFloat3(&mVelocity, acceleration);
	}


	mCamSpeed = FMath::Clamp(
							 IsKeyPressed(EKeyCode::LShift)
								 ? mCamSpeed + DeltaTime * 5.f
								 : mCamSpeed - DeltaTime * 5.f,
							 1.f,
							 10.f);


	mVelocity *= mCamSpeed;
}

void JCamera::UpdateRotation(float DeltaTime)
{
	if (IsKeyPressed(EKeyCode::LButton) || IsKeyPressed(EKeyCode::RButton))
	{
		float fPercentOfNew = 1.0f / 2.f;
		float fPercentOfOld = 1.0f - fPercentOfNew;
		mMouseDelta.x       = mMouseDelta.x * fPercentOfOld + mInputKeyboard.GetCurMouseDelta().x * fPercentOfNew;
		mMouseDelta.y       = mMouseDelta.y * fPercentOfOld + mInputKeyboard.GetCurMouseDelta().y * fPercentOfNew;

		mRotVelocity.x = mMouseDelta.x * mRotationValue;
		mRotVelocity.y = mMouseDelta.y * mRotationValue;
	}
	else
	{
		mMouseDelta.x  = 0.f;
		mMouseDelta.y  = 0.f;
		mRotVelocity.x = 0.f;
		mRotVelocity.y = 0.f;
	}
}

void JCamera::UpdateInput()
{
	mInputDirection = FVector::ZeroVector;

	if (IsKeyPressed(EKeyCode::W))
		mInputDirection.z += 1.f;
	else if (IsKeyPressed(EKeyCode::S))
		mInputDirection.z -= 1.f;
	if (IsKeyPressed(EKeyCode::D))
		mInputDirection.x += 1.f;
	else if (IsKeyPressed(EKeyCode::A))
		mInputDirection.x -= 1.f;

	// 입력이 있으면 가속 체크
	// if (!mInputDirection.IsNearlyZero())
	// {
	//
	// 	if (IsKeyPressed(EKeyCode::LButton))
	// 	{
	// 		mCamSpeed = FMath::Clamp(mCamSpeed + Application::GetDeltaSeconds() * 5.f, 1.f, 10.f);
	// 	}
	// 	else
	// 	{
	// 		mCamSpeed = FMath::Clamp(mCamSpeed - Application::GetDeltaSeconds() * 5.f, 1.f, 10.f);
	// 	}
	// }

}

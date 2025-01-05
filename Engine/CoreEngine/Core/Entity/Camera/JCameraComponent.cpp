#include "JCameraComponent.h"

#include "Core/Graphics/ShaderStructs.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"
#include "Core/Window/Application.h"

uint32_t JCameraComponent::s_CameraNum = 0;

bool FFrustum::Check(const FQuad& InQuad) const
{
	for (const auto& plane : FrustumPlanes)
	{
		const float planeToCenter =
				plane.A * InQuad.Center.x +
				plane.C * InQuad.Center.z +
				plane.D;

		const float distance =
				fabs(plane.A * InQuad.Extent.x) +
				fabs(plane.C * InQuad.Extent.z);

		if (planeToCenter <= -distance)
		{
			return false;
		}
	}
	return true;
}

bool FFrustum::Check(const FVector& InPoint) const
{
	for (const auto& FrustumPlane : FrustumPlanes)
	{
		if (FrustumPlane.A * InPoint.x +
			FrustumPlane.B * InPoint.y +
			FrustumPlane.C * InPoint.z +
			FrustumPlane.D <= 0)
		{
			return false;
		}
	}

	return true;
}

bool FFrustum::Check(const FVector& InCenter, float InRadius) const
{
	for (const auto& FrustumPlane : FrustumPlanes)
	{
		if (FrustumPlane.A * InCenter.x +
			FrustumPlane.B * InCenter.y +
			FrustumPlane.C * InCenter.z +
			FrustumPlane.D <= -InRadius)
		{
			return false;
		}
	}

	return false;
}

bool FFrustum::Check(const FBoxShape& InBox) const
{
	// 상자가 모든 평면에 대해 완전히 벗어난 경우를 확인
	for (const auto& plane : FrustumPlanes)
	{
		// 박스의 중심에서 평면까지의 거리
		const float planeToCenter =
			plane.A * InBox.Box.Center.x +
			plane.B * InBox.Box.Center.y +
			plane.C * InBox.Box.Center.z +
			plane.D;

		// 박스의 최대 거리 계산
		FVector direction = InBox.Box.LocalAxis[0] * InBox.Box.Extent.x;
		float distance = fabs(plane.A * direction.x + plane.B * direction.y + plane.C * direction.z);

		direction = InBox.Box.LocalAxis[1] * InBox.Box.Extent.y;
		distance += fabs(plane.A * direction.x + plane.B * direction.y + plane.C * direction.z);

		direction = InBox.Box.LocalAxis[2] * InBox.Box.Extent.z;
		distance += fabs(plane.A * direction.x + plane.B * direction.y + plane.C * direction.z);

		// 상자가 이 평면에서 완전히 벗어난 경우
		if (planeToCenter <= -distance)
		{
			return false; // 프러스텀 바깥
		}
	}

	// 모든 평면에 대해 벗어나지 않으면 프러스텀 안
	return true;
}

/**
 * 이름 지정이 없는 생성자의 경우 카메라 이름을 JCam_0, JCam_1, JCam_2... 순으로 지정.
 */
JCameraComponent::JCameraComponent()
	: mName(std::format(L"JCam_{}", s_CameraNum++)),
	  mDefaultEye(0, 500.f, -1500.f),
	  mDefaultLookAt(0, 0, 0),
	  mLookAt(0, 0, 0),
	  mYaw(0.f),
	  mPitch(0.f),
	  mFOV(0),
	  mAspect(0),
	  mNearPlane(1.f),
	  mFarPlane(10000.f),
	  mRotationValue(XM_PI / 180.0f),
	  mTranslationValue(500.f)
{
	mLocalLocation = mDefaultEye;

	JCameraComponent::SetViewParams(mDefaultEye, mDefaultLookAt);

	const float aspect =
			static_cast<float>(Application::s_AppInstance->GetWindowWidth()) /
			static_cast<float>(Application::s_AppInstance->GetWindowHeight());

	JCameraComponent::SetProjParams(M_PI / 3, aspect, mNearPlane, mFarPlane);

	JCameraComponent::Initialize();
}


JCameraComponent::JCameraComponent(const JTextView InName, AActor* InOwnerActor, JSceneComponent* InParentComponent)
	: JSceneComponent(InName, InOwnerActor, InParentComponent),
	  mDefaultEye(0, 500.f, -1500.f),
	  mNearPlane(10.f),
	  mFarPlane(100000.f),
	  mRotationValue(XM_PI / 180.0),
	  mTranslationValue(500.f)
{
	mLocalLocation = mDefaultEye;

	JCameraComponent::SetViewParams(mDefaultEye, mDefaultLookAt);

	const float aspect =
			static_cast<float>(Application::s_AppInstance->GetWindowWidth()) /
			static_cast<float>(Application::s_AppInstance->GetWindowHeight());

	JCameraComponent::SetProjParams(M_PI / 2.5f, aspect, mNearPlane, mFarPlane);

	JCameraComponent::Initialize();
}


JCameraComponent::~JCameraComponent() {}

void JCameraComponent::Initialize()
{
	JSceneComponent::Initialize();
}

void JCameraComponent::Tick(float_t DeltaTime)
{
	JSceneComponent::Tick(DeltaTime);

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

	// LevelManager 기저 벡터를 CameraRotation만큼 회전
	XMVECTOR worldUp       = XMVector3TransformCoord(M_UpVector, mCameraRot);
	XMVECTOR worldAhead    = XMVector3TransformCoord(M_ForwardVector, mCameraRot);
	XMVECTOR posDeltaWorld = XMVector3TransformCoord(posDelta, mCameraRot);


	XMVECTOR vEye = XMLoadFloat3(&mWorldLocation);
	vEye += posDeltaWorld;
	XMStoreFloat3(&mWorldLocation, vEye);
	SetWorldLocation(mWorldLocation);


	XMVECTOR vLookAt = vEye + worldAhead;
	XMStoreFloat3(&mLookAt, vLookAt);

	// Update the view matrix
	XMMATRIX view = XMMatrixLookAtLH(vEye, vLookAt, worldUp);
	XMStoreFloat4x4(&mView, view);

	mWorld = XMMatrixInverse(nullptr, view);

	CreateFrustum();
}

void JCameraComponent::Destroy()
{}

void JCameraComponent::PreRender(ID3D11DeviceContext* InDeviceContext)
{
	// // Front Plane
	// mVertexData[0].Position = mFrustumPosition[0];
	// mVertexData[1].Position = mFrustumPosition[1];
	// mVertexData[2].Position = mFrustumPosition[2];
	// mVertexData[3].Position = mFrustumPosition[3];
	//
	// mVertexData[0].Normal = mVertexData[1].Normal = mVertexData[2].Normal = mVertexData[3].Normal = FVector{0, 0, -1};
	// mVertexData[0].Color  = mVertexData[1].Color  = mVertexData[2].Color  = mVertexData[3].Color  = FLinearColor
	// 		{1, 0, 0, 0.5f};
	//
	// // Back Plane
	// mVertexData[4].Position = mFrustumPosition[6];
	// mVertexData[5].Position = mFrustumPosition[5];
	// mVertexData[6].Position = mFrustumPosition[1];
	// mVertexData[7].Position = mFrustumPosition[3];
	//
	// mVertexData[4].Normal = mVertexData[5].Normal = mVertexData[6].Normal = mVertexData[7].Normal = FVector{0, 0, 1};
	// mVertexData[4].Color  = mVertexData[5].Color  = mVertexData[6].Color  = mVertexData[7].Color  = FLinearColor
	// 		{0, 1, 0, 0.5f};
	//
	// // Left Plane
	// mVertexData[8].Position  = mFrustumPosition[2];
	// mVertexData[9].Position  = mFrustumPosition[6];
	// mVertexData[10].Position = mFrustumPosition[7];
	// mVertexData[11].Position = mFrustumPosition[3];
	//
	// mVertexData[8].Normal = mVertexData[9].Normal = mVertexData[10].Normal = mVertexData[11].Normal = FVector{1, 0, 0};
	// mVertexData[8].Color  = mVertexData[9].Color  = mVertexData[10].Color  = mVertexData[11].Color  = FLinearColor{
	// 	0, 0, 1, .5f
	// };
	//
	// // Right Plane
	// mVertexData[12].Position = mFrustumPosition[5];
	// mVertexData[13].Position = mFrustumPosition[1];
	// mVertexData[14].Position = mFrustumPosition[0];
	// mVertexData[15].Position = mFrustumPosition[1];
	//
	// mVertexData[12].Normal = mVertexData[13].Normal = mVertexData[14].Normal = mVertexData[15].Normal = FVector{-1, 0, 0};
	// mVertexData[12].Color  = mVertexData[13].Color  = mVertexData[14].Color  = mVertexData[15].Color  = FLinearColor{
	// 	1, 0, 1, .5f
	// };
	//
	// // Top Plane
	// mVertexData[16].Position = mFrustumPosition[5];
	// mVertexData[17].Position = mFrustumPosition[6];
	// mVertexData[18].Position = mFrustumPosition[2];
	// mVertexData[19].Position = mFrustumPosition[1];
	//
	// mVertexData[16].Normal = mVertexData[17].Normal = mVertexData[18].Normal = mVertexData[19].Normal = FVector{0, 1, 0};
	// mVertexData[16].Color  = mVertexData[17].Color  = mVertexData[18].Color  = mVertexData[19].Color  = FLinearColor{
	// 	1, 1, 0, .5f
	// };
	//
	// // Bottom Plane
	// mVertexData[20].Position = mFrustumPosition[0];
	// mVertexData[21].Position = mFrustumPosition[3];
	// mVertexData[22].Position = mFrustumPosition[7];
	// mVertexData[23].Position = mFrustumPosition[1];
	//
	// mVertexData[20].Normal = mVertexData[21].Normal = mVertexData[22].Normal = mVertexData[23].Normal = FVector{0, -1, 0};
	// mVertexData[20].Color  = mVertexData[21].Color  = mVertexData[22].Color  = mVertexData[23].Color  = FLinearColor{
	// 	0, 1, 1, .5f
	// };

	// uint32_t stride[] = {sizeof(Vertex::FVertexInfo_Simple)};
	// uint32_t offset   = 0;
	//
	// struct SimpleConstantBuffer
	// {
	// 	FMatrix World;
	// 	FMatrix View;
	// 	FMatrix Projection;
	// };
	//
	// SimpleConstantBuffer cb;
	// cb.World      = mWorld;
	// cb.View       = mView;
	// cb.Projection = mProj_Perspective;

	// InDeviceContext->UpdateSubresource(mVertexBuffer.Get(), 0, nullptr, mVertexData, 0, 0);
	//
	//
	// GetWorld.ShaderManager->ColorShader->UpdateConstantData(InDeviceContext, "SimpleConstantBuffer", &cb);
	// GetWorld.ShaderManager->UpdateShader(InDeviceContext, GetWorld.ShaderManager->ColorShader);
	// InDeviceContext->IASetVertexBuffers(0,
	// 									1,
	// 									mVertexBuffer.GetAddressOf(),
	// 									stride,
	// 									&offset);
	// InDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// InDeviceContext->DrawIndexed(24, 0, 0);
}

uint32_t JCameraComponent::GetHash() const
{
	return StringHash(ParseFile(mName.data()).c_str());
}

uint32_t JCameraComponent::GetType() const
{
	return StringHash("CameraComponent");;
}

UPtr<IManagedInterface> JCameraComponent::Clone() const
{
	return nullptr;
}

void JCameraComponent::Reset()
{
	XMStoreFloat4x4(&mWorld, XMMatrixIdentity());

	SetViewParams(mDefaultEye, mDefaultLookAt);
}

// 매개변수로 XMVECTOR가 아닌 FXMVECTOR를 사용하는 이유는
// 매개변수로 받을 때는 FXMVECTOR를 사용하고, 내부에서는 XMVECTOR를 사용하는 것이 좋다고 한다..
// 너무 많은 데이터를 레지스터에 넣을 수 있기때문(성능 저하) 
void JCameraComponent::SetViewParams(FXMVECTOR InEyeVec, FXMVECTOR InLookAtVec, FXMVECTOR InUpVec)
{
	/// 카메라가 사용하는 공간(뷰 공간)을 정의해보자.
	/// View Space의 원점은 카메라 렌즈의 정 중앙이고 이를 기준으로 3개의 축을 만들 수 있다.
	/// 이 축을 어떻게 만들 수 있을까?
	/// 카메라의 위치(eye)와 바라보는 방향(lookAt)을 알면 된다.
	/// 그 이유는 위치(eye)에서 바라보는 방향(lookAt)으로 z축을 만들고,
	/// y축은 위 방향(Up 보통은 (0,1,0))으로 만들면
	/// 외적(Cross Product)를 이용해 x축을 만들 수 있다.

	XMStoreFloat3(&mDefaultEye, InEyeVec);
	XMStoreFloat3(&mLocalLocation, InEyeVec);

	XMStoreFloat3(&mDefaultLookAt, InLookAtVec);
	XMStoreFloat3(&mLookAt, InLookAtVec);

	/// 카메라 뷰행렬 계산
	/// UP Vector는 보통 (0,1,0)을 사용한다.
	/// 왜냐? Roll(회전)을 하지 않는다면 카메라의 Up Vector는 항상 (0,1,0)에서 바뀔 이유가 없다.
	/// Roll회전을 할거면 Up Vector를 바꿔야 한다.
	XMMATRIX viewMat = XMMatrixLookAtLH(InEyeVec, InLookAtVec, InUpVec);
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

void JCameraComponent::SetProjParams(float InFOV, float InAspect, float InNearPlane, float InFarPlane)
{
	mFOV       = InFOV;
	mAspect    = InAspect;
	mNearPlane = InNearPlane;
	mFarPlane  = InFarPlane;

	XMMATRIX projMat = XMMatrixPerspectiveFovLH(InFOV, InAspect, InNearPlane, InFarPlane);
	XMStoreFloat4x4(&mProj_Perspective, projMat);

	projMat = XMMatrixOrthographicLH(1920, 1080, 0.f, 1.f);
	XMStoreFloat4x4(&mProj_Orthographic, projMat);
}

void JCameraComponent::CreateFrustum()
{
	mViewProj = XMMatrixMultiply(XMLoadFloat4x4(&mView), XMLoadFloat4x4(&mProj_Perspective));
	mViewProj = mViewProj.Invert();

	// Near Plane
	mFrustum.FrustumPosition[0] = FVector{-1, -1, 0};	// 좌측 하단
	mFrustum.FrustumPosition[1] = FVector{-1, 1, 0};	// 좌측 상단
	mFrustum.FrustumPosition[2] = FVector{1, 1, 0};	// 우측 상단
	mFrustum.FrustumPosition[3] = FVector{1, -1, 0};	// 우측 하단

	// Far Plane
	mFrustum.FrustumPosition[4] = FVector{-1, -1, 1};	// 좌측 하단
	mFrustum.FrustumPosition[5] = FVector{-1, 1, 1};	// 좌측 상단
	mFrustum.FrustumPosition[6] = FVector{1, 1, 1};	// 우측 상단
	mFrustum.FrustumPosition[7] = FVector{1, -1, 1};	// 우측 하단

	for (int32_t i = 0; i < 8; i++)
	{
		XMVECTOR v = XMLoadFloat3(&mFrustum.FrustumPosition[i]);
		v          = XMVector3TransformCoord(v, mViewProj);
		XMStoreFloat3(&mFrustum.FrustumPosition[i], v);
	}

	mFrustum.FrustumPlanes[PLANE_LEFT].CreatePlane(mFrustum.FrustumPosition[5],
												   mFrustum.FrustumPosition[0],
												   mFrustum.FrustumPosition[1]);	// Left Plane
	mFrustum.FrustumPlanes[PLANE_RIGHT].CreatePlane(mFrustum.FrustumPosition[3],
													mFrustum.FrustumPosition[6],
													mFrustum.FrustumPosition[2]);	// Right Plane
	mFrustum.FrustumPlanes[PLANE_TOP].CreatePlane(mFrustum.FrustumPosition[5],
												  mFrustum.FrustumPosition[2],
												  mFrustum.FrustumPosition[6]);	// Top Plane

	mFrustum.FrustumPlanes[PLANE_BOTTOM].CreatePlane(mFrustum.FrustumPosition[0],
													 mFrustum.FrustumPosition[7],
													 mFrustum.FrustumPosition[3]);	// Bottom Plane
	mFrustum.FrustumPlanes[PLANE_NEAR].CreatePlane(mFrustum.FrustumPosition[0],
												   mFrustum.FrustumPosition[2],
												   mFrustum.FrustumPosition[1]);	// Near Plane
	mFrustum.FrustumPlanes[PLANE_FAR].CreatePlane(mFrustum.FrustumPosition[6],
												  mFrustum.FrustumPosition[4],
												  mFrustum.FrustumPosition[5]);	// Far Plane
}

void JCameraComponent::ExtractFrustumPlanes()
{
	// // Left Plane
	// mFrustumPlanes[0].A = mViewProj._14 + mViewProj._11;
	// mFrustumPlanes[0].B = mViewProj._24 + mViewProj._21;
	// mFrustumPlanes[0].C = mViewProj._34 + mViewProj._31;
	// mFrustumPlanes[0].D = mViewProj._44 + mViewProj._41;
	//
	// // Right Plane
	// mFrustumPlanes[1].A = mViewProj._14 - mViewProj._11;
	// mFrustumPlanes[1].B = mViewProj._24 - mViewProj._21;
	// mFrustumPlanes[1].C = mViewProj._34 - mViewProj._31;
	// mFrustumPlanes[1].D = mViewProj._44 - mViewProj._41;
	//
	// // Top Plane
	// mFrustumPlanes[2].A = mViewProj._14 - mViewProj._12;
	// mFrustumPlanes[2].B = mViewProj._24 - mViewProj._22;
	// mFrustumPlanes[2].C = mViewProj._34 - mViewProj._32;
	// mFrustumPlanes[2].D = mViewProj._44 - mViewProj._42;
	//
	// // Bottom Plane
	// mFrustumPlanes[3].A = mViewProj._14 + mViewProj._12;
	// mFrustumPlanes[3].B = mViewProj._24 + mViewProj._22;
	// mFrustumPlanes[3].C = mViewProj._34 + mViewProj._32;
	// mFrustumPlanes[3].D = mViewProj._44 + mViewProj._42;
	//
	// // Near Plane
	// mFrustumPlanes[4].A = mViewProj._13;
	// mFrustumPlanes[4].B = mViewProj._23;
	// mFrustumPlanes[4].C = mViewProj._33;
	// mFrustumPlanes[4].D = mViewProj._43;
	//
	// // Far Plane
	// mFrustumPlanes[5].A = mViewProj._14 - mViewProj._13;
	// mFrustumPlanes[5].B = mViewProj._24 - mViewProj._23;
	// mFrustumPlanes[5].C = mViewProj._34 - mViewProj._33;
	// mFrustumPlanes[5].D = mViewProj._44 - mViewProj._43;
}

FVector JCameraComponent::GetFlatForwardVector() const
{
	// mCameraAhead에서 Y축 성분 제거
	FVector flatForward = mCameraAhead;
	flatForward.y       = 0.0f; // Y축 제거
	flatForward.Normalize();
	return flatForward;
}

FVector JCameraComponent::GetFlatRightVector() const
{
	FVector flatRight = mCameraRight;
	flatRight.y       = 0.0f;
	flatRight.Normalize();
	return flatRight;
}


void JCameraComponent::UpdateVelocity(float DeltaTime)
{
	XMVECTOR vMouseDelta  = XMLoadFloat2(&mMouseDelta);
	XMVECTOR vRotVelocity = vMouseDelta * mRotationValue;

	XMStoreFloat2(&mRotVelocity, vRotVelocity);
}

void JCameraComponent::UpdateRotation(float DeltaTime)
{
	mMouseDelta.x  = 0.f;
	mMouseDelta.y  = 0.f;
	mRotVelocity.x = 0.f;
	mRotVelocity.y = 0.f;
}

JCamera_Debug::JCamera_Debug()
	: JCameraComponent(),
	  mInputKeyboard()
{
	mInputKeyboard.Initialize();
}

JCamera_Debug::JCamera_Debug(JTextView InName)
	: JCameraComponent(InName),
	  mInputKeyboard()
{
	mInputKeyboard.Initialize();
}


void JCamera_Debug::Initialize()
{}

void JCamera_Debug::Tick(float DeltaTime)
{
	UpdateInput(DeltaTime);

	JCameraComponent::Tick(DeltaTime);
}

void JCamera_Debug::Destroy()
{
	JCameraComponent::Destroy();
}

void JCamera_Debug::UpdateInput(float DeltaTime)
{
	mInputKeyboard.Update(DeltaTime);

	mInputDirection = FVector::ZeroVector;

	if (IsKeyPressed(EKeyCode::W))
		mInputDirection.z += 1.f;
	else if (IsKeyPressed(EKeyCode::S))
		mInputDirection.z -= 1.f;
	if (IsKeyPressed(EKeyCode::D))
		mInputDirection.x += 1.f;
	else if (IsKeyPressed(EKeyCode::A))
		mInputDirection.x -= 1.f;

	if (IsKeyPressed(EKeyCode::Home))
	{
		mYaw           = 0.f;
		mPitch         = 0.f;
		mLocalLocation = mDefaultEye;
	}
}

void JCamera_Debug::UpdateVelocity(float DeltaTime)
{
	JCameraComponent::UpdateVelocity(DeltaTime);

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
								 ? mCamSpeed + DeltaTime * mCamSpeed
								 : mCamSpeed - DeltaTime * mCamSpeed,
							 1.f,
							 50.f);


	mVelocity *= mCamSpeed;
}

void JCamera_Debug::UpdateRotation(float DeltaTime)
{
	JCameraComponent::UpdateRotation(DeltaTime);

	if (IsKeyPressed(EKeyCode::RButton))
	{
		float fPercentOfNew = 1.0f / 2.f;
		float fPercentOfOld = 1.0f - fPercentOfNew;
		mMouseDelta.x       = mMouseDelta.x * fPercentOfOld + mInputKeyboard.GetCurMouseDelta().x * fPercentOfNew;
		mMouseDelta.y       = mMouseDelta.y * fPercentOfOld + mInputKeyboard.GetCurMouseDelta().y * fPercentOfNew;

		mRotVelocity.x = mMouseDelta.x * mRotationValue;
		mRotVelocity.y = mMouseDelta.y * mRotationValue;
	}
}

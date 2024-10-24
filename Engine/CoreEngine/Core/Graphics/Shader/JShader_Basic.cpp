#include "JShader_Basic.h"
#include "Core/Entity/Component/Scene//JSceneComponent.h"
#include "Core/Entity/Camera/JCamera.h"
#include "Core/Interface/MManagerInterface.h"

JShader_Basic::JShader_Basic(const JText& InName)
	: JShader(NAME_SHADER_BASIC)
{
	assert(mShaderData.ConstantBufferHashTable.contains(CBuffer::NAME_CONSTANT_BUFFER_SPACE));
	assert(mShaderData.ConstantBufferHashTable.contains(CBuffer::NAME_CONSTANT_BUFFER_LIGHT));
	assert(mShaderData.ConstantBufferHashTable.contains(CBuffer::NAME_CONSTANT_BUFFER_CAMERA));

	mSpaceConstantBufferIndex  = mShaderData.ConstantBufferHashTable[CBuffer::NAME_CONSTANT_BUFFER_SPACE];
	mLightConstantBufferIndex  = mShaderData.ConstantBufferHashTable[CBuffer::NAME_CONSTANT_BUFFER_LIGHT];
	mCameraConstantBufferIndex = mShaderData.ConstantBufferHashTable[CBuffer::NAME_CONSTANT_BUFFER_CAMERA];

	mLightData.LightPos   = FVector4{500, 1200, 1000, 1};
	mLightData.LightColor = FVector4{1.f, 0.605063f, 0.224581f, 1}; // 6500k 주광색 (완벽히 같진 않음)
}

void JShader_Basic::SetObject(ID3D11DeviceContext* InDeviceContext, const FMatrix& InTransform)
{
	mSpaceData.Model = XMMatrixTranspose(InTransform);
	mShaderData.ConstantBuffers[mSpaceConstantBufferIndex].UpdateConstantData(
																			  InDeviceContext,
																			  mSpaceData
																			 );
}

void JShader_Basic::SetDirectionalLightPos(ID3D11DeviceContext* InDeviceContext, const FVector4& InPos)
{
	mLightData.LightPos = InPos;
	mShaderData.ConstantBuffers[mLightConstantBufferIndex].UpdateConstantData(
																			  InDeviceContext,
																			  InPos
																			 );
}

void JShader_Basic::SetDirectionalLightColor(ID3D11DeviceContext* InDeviceContext, const FVector4& InColor)
{
	mLightData.LightColor = InColor;
	mShaderData.ConstantBuffers[mLightConstantBufferIndex].UpdateConstantData(
																			  InDeviceContext,
																			  InColor
																			 );
}

void JShader_Basic::SetCameraData(ID3D11DeviceContext* InDeviceContext, const Ptr<JCamera>& InCamera)
{
	mTargetCamera         = InCamera;
	mCameraData.CameraPos = FVector4{mTargetCamera->GetEyePositionFVector(), 1.f};
	mShaderData.ConstantBuffers[mCameraConstantBufferIndex].UpdateConstantData(
																			   InDeviceContext,
																			   mCameraData
																			  );
}

void JShader_Basic::UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext)
{
	if (!mTargetCamera)
	{
		mTargetCamera = IManager.CameraManager->GetCurrentMainCam();
	}

	mSpaceData.View       = XMMatrixTranspose(mTargetCamera->GetViewMatrix());
	mSpaceData.Projection = XMMatrixTranspose(mTargetCamera->GetProjMatrix());
	// 상수버퍼 넘겨주기 전에 상수버퍼 업데이트 (자동화)
	mShaderData.ConstantBuffers[mSpaceConstantBufferIndex].UpdateConstantData(
																			  InDeviceContext,
																			  mSpaceData
																			 );
	FVector camPosition = mTargetCamera->GetEyePositionFVector();
	mShaderData.ConstantBuffers[mCameraConstantBufferIndex].UpdateConstantData(
																			   InDeviceContext,
																			   camPosition);

	// 상수버퍼 넘겨주기
	JShader::UpdateGlobalConstantBuffer(InDeviceContext);
}

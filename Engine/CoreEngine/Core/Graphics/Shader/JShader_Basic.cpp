#include "JShader_Basic.h"
#include "Core/Entity/Camera/JCamera.h"
#include "Core/Entity/Component/Scene//JSceneComponent.h"
#include "Core/Interface/MManagerInterface.h"

extern CBuffer::Light g_LightData;

JShader_Basic::JShader_Basic(const JText& InName)
	: JShader(NAME_SHADER_BASIC)
{}

void JShader_Basic::UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext)
{
	if (!mTargetCamera)
	{
		mTargetCamera = IManager.CameraManager->GetCurrentMainCam();
	}

	if (const auto viewMat = XMMatrixTranspose(mTargetCamera->GetViewMatrix()); mCachedSpaceData.View != viewMat)
	{
		mCachedSpaceData.View = viewMat;
		UpdateConstantData(InDeviceContext,
						   CBuffer::NAME_CONSTANT_BUFFER_SPACE,
						   CBuffer::NAME_CONSTANT_VARIABLE_SPACE_VIEW,
						   &mCachedSpaceData.View);
	}

	if (const auto projMat = XMMatrixTranspose(mTargetCamera->GetProjMatrix()); mCachedSpaceData.Projection != projMat)
	{
		mCachedSpaceData.Projection = projMat;
		UpdateConstantData(InDeviceContext,
						   CBuffer::NAME_CONSTANT_BUFFER_SPACE,
						   CBuffer::NAME_CONSTANT_VARIABLE_SPACE_PROJ,
						   &mCachedSpaceData.Projection);
	}

	if (mCachedLightData.LightColor != g_LightData.LightColor || mCachedLightData.LightPos != g_LightData.LightPos)
	{
		mCachedLightData = g_LightData;
		UpdateConstantData(InDeviceContext, CBuffer::NAME_CONSTANT_BUFFER_LIGHT, &g_LightData);
	}

	if (const auto camPos = FVector4(mTargetCamera->GetEyePositionFVector(), 1.0f); mCachedCameraData.CameraPos != camPos)
	{
		mCachedCameraData.CameraPos = camPos;
		UpdateConstantData(InDeviceContext, CBuffer::NAME_CONSTANT_BUFFER_CAMERA, &mCachedCameraData);
	}

	// 상수버퍼 넘겨주기
	JShader::UpdateGlobalConstantBuffer(InDeviceContext);

}

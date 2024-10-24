#pragma once
#include "JShader.h"

class JSceneComponent;
class JCamera;

class JShader_Basic : public JShader
{
public:
	JShader_Basic(const JText& InName);
	~JShader_Basic() override = default;

public:
	void SetObject(ID3D11DeviceContext* InDeviceContext, const FMatrix& InTransform);
	void SetDirectionalLightPos(ID3D11DeviceContext* InDeviceContext, const FVector4& InPos);
	void SetDirectionalLightColor(ID3D11DeviceContext* InDeviceContext, const FVector4& InColor);
	void SetCameraData(ID3D11DeviceContext* InDeviceContext, const Ptr<JCamera>& InCamera);

	void UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext) override;

public:
	FVector4 GetDirectionalLightPos() const { return mLightData.LightPos; }
	FVector4 GetDirectionalLightColor() const { return mLightData.LightColor; }

private:
	CBuffer::Space  mSpaceData;
	CBuffer::Light  mLightData;
	CBuffer::Camera mCameraData;

	Ptr<JCamera> mTargetCamera;		// 오브젝트가 참조하는 카메라 개체 

	int32_t mSpaceConstantBufferIndex  = -1;
	int32_t mLightConstantBufferIndex  = -1;
	int32_t mCameraConstantBufferIndex = -1;
};

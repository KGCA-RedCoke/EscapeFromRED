#pragma once
#include "JShader.h"

class JSceneComponent;
class JCamera;

class JShader_Basic : public JShader
{
public:
	JShader_Basic(const JText& InName);
	~JShader_Basic() override = default;

	void UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext) override;
	void SetTargetCamera(const Ptr<JCamera>& InCamera) { mTargetCamera = InCamera; }

private:
	Ptr<JCamera> mTargetCamera;		// 오브젝트가 참조하는 카메라 개체

	// ------------------------------ 캐싱 데이터 ------------------------------
	CBuffer::Space  mCachedSpaceData;
	CBuffer::Light  mCachedLightData;
	CBuffer::Camera mCachedCameraData;

};

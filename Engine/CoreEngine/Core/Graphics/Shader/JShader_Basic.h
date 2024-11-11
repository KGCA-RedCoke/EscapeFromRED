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

private:

	// ------------------------------ 캐싱 데이터 ------------------------------
	CBuffer::Light  mCachedLightData;

};

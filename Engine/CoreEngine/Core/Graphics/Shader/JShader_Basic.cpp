#include "JShader_Basic.h"
#include "Core/Interface/JWorld.h"

extern CBuffer::Light g_LightData;

JShader_Basic::JShader_Basic(const JText& InName)
	: JShader(NAME_SHADER_BASIC)
{}

void JShader_Basic::UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext)
{
	if (mCachedLightData.LightColor != g_LightData.LightColor || mCachedLightData.LightPos != g_LightData.LightPos)
	{
		mCachedLightData = g_LightData;
		UpdateConstantData(InDeviceContext, CBuffer::NAME_CONSTANT_BUFFER_LIGHT, &g_LightData);
	}

	// 상수버퍼 넘겨주기
	JShader::UpdateGlobalConstantBuffer(InDeviceContext);

}

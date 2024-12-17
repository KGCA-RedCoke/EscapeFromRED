#include "JShader_Basic.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"
#include "GUI/MGUIManager.h"


JShader_Basic::JShader_Basic(const JText& InName)
	: JShader(NAME_SHADER_BASIC)
{}

void JShader_Basic::BindShaderPipeline(ID3D11DeviceContext* InDeviceContext)
{
	JShader::BindShaderPipeline(InDeviceContext);
}

void JShader_Basic::UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext)
{
	// 상수버퍼 넘겨주기
	JShader::UpdateGlobalConstantBuffer(InDeviceContext);

}

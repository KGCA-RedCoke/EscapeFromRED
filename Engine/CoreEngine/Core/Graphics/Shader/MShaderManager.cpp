#include "MShaderManager.h"

#include "InputLayouts.h"
#include "JShader_Basic.h"

MShaderManager::MShaderManager()
{
	Initialize_Initialize();
}

void MShaderManager::Initialize_Initialize()
{
	BasicShader = CreateOrLoad<JShader_Basic>(NAME_SHADER_BASIC);
	CreateOrLoad(NAME_SHADER_GNOMON);
}

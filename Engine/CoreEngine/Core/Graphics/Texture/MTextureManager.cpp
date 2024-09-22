#include "MTextureManager.h"


void MTextureManager::Initialize()
{
	DefaultAlbedoTexture = CreateOrLoad("rsc/Engine/Material/Default/Default_Albedo.jpg");
	DefaultNormalTexture = CreateOrLoad("rsc/Engine/Material/Default/Default_Normal.jpg");
}

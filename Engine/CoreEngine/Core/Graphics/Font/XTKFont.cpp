#include "XTKFont.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/MManagerInterface.h"

void XTKFont::Initialize()
{
	mSpriteBatch = std::make_unique<SpriteBatch>(IManager.RenderManager->GetImmediateDeviceContext());
	mSpriteFont  = std::make_unique<SpriteFont>(IManager.RenderManager->GetDevice(), L"");
}

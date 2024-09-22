#include "XTKFont.h"

#include "Core/Graphics/XD3DDevice.h"

void XTKFont::Initialize()
{
	mSpriteBatch = std::make_unique<SpriteBatch>(DeviceRSC.GetImmediateDeviceContext());
	mSpriteFont  = std::make_unique<SpriteFont>(DeviceRSC.GetDevice(), L"");
}

#include "common_pch.h"
#include "XTKFont.h"

#include "Core/Graphics/GraphicDevice.h"

void XTKFont::Initialize()
{
	mSpriteBatch = std::make_unique<SpriteBatch>(G_Context.GetImmediateDeviceContext());
	mSpriteFont  = std::make_unique<SpriteFont>(G_Context.GetDevice(), L"");
}

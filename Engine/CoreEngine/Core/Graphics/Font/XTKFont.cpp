#include "XTKFont.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"

void XTKFont::Initialize()
{
	mSpriteBatch = std::make_unique<SpriteBatch>(GetWorld.D3D11API->GetImmediateDeviceContext());
	mSpriteFont  = std::make_unique<SpriteFont>(GetWorld.D3D11API->GetDevice(), L"");
}

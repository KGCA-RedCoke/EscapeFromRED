#pragma once
#include <directxtk/SpriteFont.h>
#include "Core/Graphics/graphics_common_include.h"

class XTKFont
{
public:
	void Initialize();

private:
	std::unique_ptr<SpriteBatch> mSpriteBatch;
	std::unique_ptr<SpriteFont>  mSpriteFont;
};

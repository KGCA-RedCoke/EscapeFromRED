#pragma once

#include "Core/Graphics/graphics_common_include.h"
#include "Core/Interface/ICoreInterface.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/Math/Color.h"
#include "Core/Utils/Math/Vector2.h"


class JFont : public ICoreInterface, public IRenderable
{
public:
	JFont();
	explicit JFont(ID2D1RenderTarget* InRenderTarget = nullptr);
	virtual  ~JFont() override;

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Release() override;
#pragma endregion

#pragma region Render Interface
	void PreRender() override;
	void AddInstance(float InCameraDistance) override;
	void PostRender() override;
	void Draw() override;
	void DrawID(uint32_t ID) override {};
#pragma endregion

public:
	void SetRenderTarget(ID2D1RenderTarget* InRenderTarget);
	void SetText(const JWText& InText);
	void SetColor(const struct FLinearColor& InColor);
	void SetFontFamily(const JWText& InFontFamily);
	void SetFontWeight(const DWRITE_FONT_WEIGHT InFontWeight);
	void SetFontStyle(const DWRITE_FONT_STYLE InFontStyle);
	void SetFontStretch(const DWRITE_FONT_STRETCH InFontStretch);
	void SetFontSize(const float InFontSize);

	void SetScreenPosition(const JMath::TVector2& InLocation);
	void SetNDCPosition(const JMath::TVector2& InLocation);

	JWText       GetText() const { return mText; };
	FLinearColor GetColor() const { return mBrushColor; }
	float        GetFontSize() const { return mFontSize; }

private:
	void AdjustTextFormat();

private:
	ComPtr<IDWriteTextFormat> mTextFormat   = nullptr;
	ID2D1RenderTarget*        mRenderTarget = nullptr;
	JWText                    mFontFamily;
	DWRITE_FONT_WEIGHT        mFontWeight  = DWRITE_FONT_WEIGHT_NORMAL;
	DWRITE_FONT_STYLE         mFontStyle   = DWRITE_FONT_STYLE_NORMAL;
	DWRITE_FONT_STRETCH       mFontStretch = DWRITE_FONT_STRETCH_NORMAL;
	float                     mFontSize    = 12.f;
	JWText                    mText;
	FLinearColor              mBrushColor = FLinearColor::White;

	JMath::TVector2 mScreenPosition;
};

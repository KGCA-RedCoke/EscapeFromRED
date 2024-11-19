#include "JFont.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/Math/Color.h"

JFont::JFont(ID2D1RenderTarget* InRenderTarget)
	: mRenderTarget(InRenderTarget),
	  mFontFamily(L"Gabriola"),
	  mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
	  mFontStyle(DWRITE_FONT_STYLE_NORMAL),
	  mFontStretch(DWRITE_FONT_STRETCH_NORMAL),
	  mFontSize(18),
	  mBrushColor({0, 0, 0, 1})
{
}

JFont::~JFont() {}

void JFont::Initialize()
{
	AdjustTextFormat();
}

void JFont::Update(float_t DeltaTime) {}

void JFont::PreRender()
{
	// Draw 호출 전 Begin
	mRenderTarget->BeginDraw();
}

void JFont::Render()
{
	ID2D1SolidColorBrush* brush = GetWorld.D3D11API->GetBrush();
	assert(brush, "invalid brush");

	D2D1_COLOR_F brushColor;
	brushColor.r = mBrushColor.R;
	brushColor.g = mBrushColor.G;
	brushColor.b = mBrushColor.B;
	brushColor.a = mBrushColor.A;
	brush->SetColor(brushColor);

	mRenderTarget->DrawTextW(
							 mText.c_str(),
							 mText.length(),
							 mTextFormat.Get(),
							 {
								 mScreenPosition.x,
								 mScreenPosition.y,
								 mScreenPosition.x + 600.f,
								 mScreenPosition.y + 600.f
							 },
							 GetWorld.D3D11API->GetBrush()
							);
}

void JFont::PostRender()
{
	// Draw 호출 후 End
	CheckResult(mRenderTarget->EndDraw());
}

void JFont::SetRenderTarget(ID2D1RenderTarget* InRenderTarget)
{
	mRenderTarget = InRenderTarget;
}

void JFont::Release()
{
	mTextFormat = nullptr;
}


void JFont::SetText(const JWText& InText)
{
	mText = InText;
}

void JFont::SetColor(const FLinearColor& InColor)
{
	mBrushColor = InColor;
}

void JFont::SetFontFamily(const JWText& InFontFamily)
{
	mFontFamily = InFontFamily.c_str();

	AdjustTextFormat();
}

void JFont::SetFontStyle(const DWRITE_FONT_STYLE InFontStyle)
{
	mFontStyle = InFontStyle;

	AdjustTextFormat();
}

void JFont::SetFontStretch(const DWRITE_FONT_STRETCH InFontStretch)
{
	mFontStretch = InFontStretch;

	AdjustTextFormat();
}

void JFont::SetFontWeight(const DWRITE_FONT_WEIGHT InFontWeight)
{
	mFontWeight = InFontWeight;

	AdjustTextFormat();
}

void JFont::SetFontSize(const float InFontSize)
{
	mFontSize = InFontSize;

	AdjustTextFormat();
}

void JFont::SetScreenPosition(const JMath::TVector2& InLocation)
{
	mScreenPosition = InLocation;
}

void JFont::AdjustTextFormat()
{
	mTextFormat = nullptr;

	// DWrite에서는 Format만 관리
	CheckResult(
				GetWorld.D3D11API->GetWriteFactory()->
						  CreateTextFormat(
										   mFontFamily.c_str(),
										   nullptr,
										   mFontWeight,
										   mFontStyle,
										   mFontStretch,
										   mFontSize,
										   L"en-us",
										   mTextFormat.GetAddressOf()
										  ));
}

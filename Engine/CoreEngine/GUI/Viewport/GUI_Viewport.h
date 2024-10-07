#pragma once

#include "Core/Utils/Math/Vector2.h"
#include "GUI/GUI_Base.h"

class JCamera;

class GUI_Viewport : public GUI_Base
{
public:
	explicit GUI_Viewport(const JText& InTitle);
	~GUI_Viewport() override = default;

protected:
	void Update_Implementation(float DeltaTime) override;

public:
	FORCEINLINE void SetCamera(const Ptr<JCamera>& InCamera) { mEditorCameraRef = InCamera; }

	[[nodiscard]] FORCEINLINE bool IsFocused() const { return bIsFocused; }
	[[nodiscard]] FORCEINLINE bool IsHovered() const { return bIsHovered; }

protected:
	bool bIsFocused;
	bool bIsHovered;

	Ptr<JCamera> mEditorCameraRef;

	JText    mViewportTitle;
	FVector2 mCachedViewportSize;
};

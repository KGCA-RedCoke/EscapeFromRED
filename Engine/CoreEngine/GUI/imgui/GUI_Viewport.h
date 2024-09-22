#pragma once

#include "GUI_Base.h"
#include "Core/Utils/Math/Vector2.h"

class JCamera;

class GUI_Viewport : public GUI_Base
{
public:
	explicit GUI_Viewport(const std::string& InTitle);
	~GUI_Viewport() override;

public:
	void Initialize() override;
	void Update(float DeltaTime) override;

public:
	[[nodiscard]] FORCEINLINE bool IsBIsFocused() const { return bIsFocused; }
	[[nodiscard]] FORCEINLINE bool IsBIsHovered() const { return bIsHovered; }

protected:
	bool bIsFocused;
	bool bIsHovered;

	JCamera* mEditorCameraRef;
	FVector2 mCachedViewportSize;
};

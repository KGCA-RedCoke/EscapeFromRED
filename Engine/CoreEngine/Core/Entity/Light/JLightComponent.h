#pragma once
#include "Core/Entity/Component/Scene/JSceneComponent.h"

enum class ELightType
{
	Directional,
	Point,
	Spot,
	Rect,
	Disc,
	Sphere,
	Capsule,
};

class JLightComponent : public JSceneComponent
{
public:
	JLightComponent();

public:
	void SetLightColor(const FVector4& InColor) { mLightColor = InColor; }
	void SetIntensity(float InIntensity) { mIntensity = InIntensity; }

	[[nodiscard]] ELightType      GetLightType() const { return mLightType; }
	[[nodiscard]] const FVector4& GetLightColor() const { return mLightColor; }
	[[nodiscard]] float           GetIntensity() const { return mIntensity; }

protected:
	ELightType mLightType;
	FVector4   mLightColor;
	float      mIntensity;
};

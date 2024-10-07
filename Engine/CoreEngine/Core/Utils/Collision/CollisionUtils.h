#pragma once
#include "Core/Graphics/graphics_common_include.h"

namespace Utils::Collision
{
	bool RayIntersectsAABB(const FVector& RayOrigin,
						   const FVector& RayDirection,
						   const FVector& BoxMin,
						   const FVector& BoxMax,
						   float&         OutDistance);
}

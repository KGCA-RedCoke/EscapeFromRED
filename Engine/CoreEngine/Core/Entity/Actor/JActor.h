#pragma once
#include "Core/Entity/Transform/JTransformComponent.h"


class JActor : public JTransformComponent
{
public:
#pragma region Render Interface
	void PreRender();
	void Render();
	void PostRender();
#pragma endregion


};

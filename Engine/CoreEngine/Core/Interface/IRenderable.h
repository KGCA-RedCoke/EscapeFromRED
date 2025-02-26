﻿#pragma once
#include <cstdint>

enum class ELayerType : uint32_t
{
	GameObject = 0,
	UI,
	GUI,
	Background_2D,
	Foreground_2D,
	Player_2D,
	Particle_2D,
	End
};

class IRenderable
{
public:
	/** Draw 호출 전 사전 작업 */
	virtual void PreRender() = 0;
	/** Render 순서 정렬 등 최종 Draw전 작업 */
	virtual void AddInstance(float InCameraDistance) = 0;
	/** Draw or DrawIndex 호출 */
	virtual void PostRender() = 0;

	virtual void Draw() = 0;

	virtual void DrawID(uint32_t ID) = 0;

	virtual uint32_t GetIndexCount() const { return 0; }
};

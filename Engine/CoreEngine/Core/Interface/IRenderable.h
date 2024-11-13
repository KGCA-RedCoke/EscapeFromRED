#pragma once
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

/**
 * 간단한 Draw로직이 필요한 개체들의 인터페이스
 */
class IRenderable
{
public:
	virtual ~IRenderable() = default;

	/** 여기에 구현 */
	virtual void Draw(ID3D11DeviceContext* InDeviceContext) = 0;

	/** 고유 ID를 구분하기 위한 Draw */
	virtual void DrawID(ID3D11DeviceContext* InDeviceContext, uint32_t ID) = 0;
};

/**
 * 인스턴싱이 필요한 개체들의 인터페이스
 */
class IRenderable_Instance : public IRenderable
{
	/** Draw 호출 전 사전 작업 */
	virtual void PreRender() = 0;
	/** Render 순서 정렬 등 최종 Draw전 작업 */
	virtual void Render() = 0;
	/** Draw or DrawIndex 호출 */
	virtual void PostRender() = 0;

	virtual uint32_t GetIndexCount() const = 0;

	virtual void UpdateInstanceBuffer(class ID3D11Device*        Device,
									  class ID3D11DeviceContext* InDeviceContext,
									  uint32_t                   InInstanceCount) = 0;
};

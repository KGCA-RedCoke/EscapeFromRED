#pragma once
#include <map>

#include "Core/Manager/Manager_Base.h"

class IRenderable;
class XLayerBase;

class Renderer : public TSingleton<Renderer>
{
public:
	void Render();

public:
	void AddLayerObject(IRenderable* InRenderableObj);
	

private:
	std::map<uint32_t, std::unique_ptr<XLayerBase>> mLayers;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<Renderer>;

	Renderer();

public:
	Renderer(const Renderer&)            = delete;
	Renderer& operator=(const Renderer&) = delete;

#pragma endregion
};

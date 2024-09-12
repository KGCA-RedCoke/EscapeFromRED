#include "common_pch.h"
#include "Renderer.h"

#include <ranges>

#include "GraphicDevice.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Math/Color.h"
#include "Layer/XLayerBase.h"

Renderer::Renderer()
{}

void Renderer::Render()
{
	G_Context.ClearColor(FLinearColor::Gallary);

	IManager.LayerManager.Render();

	G_Context.Present();
}

void Renderer::AddLayerObject(IRenderable* InRenderableObj)
{
	// auto it = mLayers.find(static_cast<uint32_t>(InRenderableObj->GetLayerType()));
	//
	// if (it != mLayers.end())
	// {
	// 	it->second->AddLayerObject(InRenderableObj);
	// }
}

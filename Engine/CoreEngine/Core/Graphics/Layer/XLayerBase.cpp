#include "XLayerBase.h"

#include "Core/Utils/Logger.h"

XLayerBase::XLayerBase(ELayerType LayerType)
	: mLayerType(LayerType) {}

XLayerBase::~XLayerBase() = default;

void XLayerBase::Render() const
{
	const int32_t objNum = mRenderObjects.size();

	for (int32_t i = 0; i < objNum; ++i)
	{
		mRenderObjects[i]->PreRender();

		mRenderObjects[i]->AddInstance();

		mRenderObjects[i]->PostRender();
	}
}

void XLayerBase::AddLayerObject(IRenderable* RenderableObj)
{
	if (RenderableObj)
	{
		mRenderObjects.emplace_back(RenderableObj);
	}
	else
	{
		LOG_CORE_ERROR("ItemType and LayerType MisMatch... %s", __FUNCTION__);
	}
}

void XLayerBase::DeleteLayerObject(IRenderable* RenderableObj)
{
	auto it = std::ranges::find(
								mRenderObjects,
								RenderableObj);
	if (it != mRenderObjects.end())
	{
		mRenderObjects.erase(it);
	}
}

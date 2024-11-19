#pragma once
#include "Core/Interface/IRenderable.h"
#include "Core/Manager/Manager_Base.h"


class XLayerBase
{
public:
	explicit XLayerBase(ELayerType LayerType);
	~XLayerBase();

public:
	void Render() const;

	[[nodiscard]] FORCEINLINE ELayerType GetLayerType() const { return mLayerType; }

public:
	void AddLayerObject(IRenderable* RenderableObj);
	void DeleteLayerObject(IRenderable* RenderableObj);

protected:
	ELayerType                mLayerType;
	std::vector<IRenderable*> mRenderObjects;

};


class MLayerManager : public TSingleton<MLayerManager>
{
public:
	void Render()
	{
		for (int32_t i = 0; i < static_cast<int32_t>(ELayerType::End); ++i)
		{
			mLayers[i]->Render();
		}
	}

	void AddLayerObject(IRenderable* InRenderable)
	{
		// const uint32_t index = static_cast<uint32_t>(InRenderable->GetLayerType());

		// if (mLayers.size() >= index)
		// {
		// 	mLayers[index]->AddLayerObject(InRenderable);
		// }
	}

private:
	void CreateLayers()
	{
		mLayers.reserve(static_cast<uint32_t>(ELayerType::End));

		for (int32_t i = 0; i < static_cast<int32_t>(ELayerType::End); ++i)
		{
			mLayers.push_back(std::make_unique<XLayerBase>(static_cast<ELayerType>(i)));
		}
	}

private:
	std::vector<std::unique_ptr<XLayerBase>> mLayers;

private:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MLayerManager>;
	friend class JWorld;

	MLayerManager() { CreateLayers(); }

public:
	MLayerManager(const MLayerManager&)            = delete;
	MLayerManager& operator=(const MLayerManager&) = delete;

#pragma endregion
};

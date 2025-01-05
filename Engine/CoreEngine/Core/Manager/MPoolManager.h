#pragma once
#include "Manager_Base.h"


template <class ClassType>
class MPoolManager : public TSingleton<MPoolManager<ClassType>>
{
public:
	MPoolManager();
	~MPoolManager();

	template <typename... Args>
	UPtr<ClassType> Spawn(Args&&... InArgs);

	void DeSpawn(UPtr<ClassType> Object);

protected:
	int32_t                 mMaxPoolSize = 100.f;
	JArray<UPtr<ClassType>> mPoolObjects;
};

template <class ClassType>
MPoolManager<ClassType>::MPoolManager()
{}

template <class ClassType>
MPoolManager<ClassType>::~MPoolManager() {}

template <class ClassType>
template <typename... Args>
UPtr<ClassType> MPoolManager<ClassType>::Spawn(Args&&... InArgs)
{
	if (mPoolObjects.empty())
	{
		for (size_t i = 0; i < mMaxPoolSize; ++i)
		{
			mPoolObjects.emplace_back(std::make_unique<ClassType>(std::forward<Args>(InArgs)...));
			mPoolObjects[i]->SetActive(false);
		}
	}
	UPtr<ClassType> obj = std::move(mPoolObjects.back());
	obj->SetActive(true);
	mPoolObjects.pop_back();
	return obj;
}

template <class ClassType>
void MPoolManager<ClassType>::DeSpawn(UPtr<ClassType> Object)
{
	Object->SetActive(false);
	mPoolObjects.emplace_back(std::move(Object));
}

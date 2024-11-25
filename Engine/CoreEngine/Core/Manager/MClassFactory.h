#pragma once
#include "Manager_Base.h"

class JObject;
using CreatorFunc = std::function<UPtr<JObject>()>;

class MClassFactory : public TSingleton<MClassFactory>
{
public:
	void Register(const JText& InClassName, CreatorFunc InCreator)
	{
		mRegisteredClass[InClassName] = std::move(InCreator);
	}

	UPtr<class JObject> Create(const JText& InClassName) const
	{
		auto it = mRegisteredClass.find(InClassName);

		if (it != mRegisteredClass.end())
		{
			return it->second();
		}

		return nullptr;
	}

private:
	JHash<JText, CreatorFunc> mRegisteredClass;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MClassFactory>;

	MClassFactory()  = default;
	~MClassFactory() = default;

public:
	MClassFactory(const MClassFactory&)            = delete;
	MClassFactory& operator=(const MClassFactory&) = delete;

#pragma endregion
};

#define REGISTER_CLASS_TYPE(type) \
	namespace {                                                     \
	struct type##AutoRegister                                       \
	{                                                               \
		type##AutoRegister()                                        \
		{                                                           \
			MClassFactory::Get().Register(#type, []() {             \
				return MakeUPtr<type>();                    \
			});                                                     \
		}                                                           \
	};                                                              \
	static type##AutoRegister global_##type##AutoRegisterInstance;  \
}

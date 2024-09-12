#pragma once
#include "Core/Utils/HelperFunction.h"
#include "Core/Utils/Utils.h"

template <class SingletonType>
class TSingleton
{
public:
	[[nodiscard]] static SingletonType& Get()
	{
		static SingletonType instance;
		return instance;
	}

	// Child Class에 넣자
	/*
	#pragma region Singleton Boilerplate
		
		private:
			friend class TSingleton<SingletonType>;
		
			SingletonType();
			~SingletonType() override;
		
		public:
			SingletonType(const SingletonType&)            = delete;
			SingletonType& operator=(const SingletonType&) = delete;
		
	#pragma endregion
	*/
};

template <class ManagedType, class Manager>
class Manager_Base : public TSingleton<Manager>
{
public:
	template <class ReturnType = ManagedType, typename... Args>
	ReturnType* CreateOrLoad(const std::wstring& InName, Args&&... InArgs);
	template <class ReturnType = ManagedType, typename... Args>
	ReturnType* CreateOrLoad(const std::string& InName, Args&&... InArgs);

	template <class ReturnType = ManagedType>
	ReturnType* FetchResource(const std::wstring& InName);
	template <class ReturnType = ManagedType>
	ReturnType* FetchResource(const std::string& InName);

	void SafeRemove(const std::wstring& InName);
	void SafeRemove(const std::string& InName);

protected:
	std::unordered_map<uint32_t, std::unique_ptr<ManagedType>> mManagedList;
};

template <class ManagedType, class Manager>
template <class ReturnType, typename... Args>
ReturnType* Manager_Base<ManagedType, Manager>::CreateOrLoad(const std::wstring& InName, Args&&... InArgs)
{
	std::wstring id   = ParseFile(InName);
	uint32_t     hash = StringHash(id.c_str());

	if (ReturnType* resource = FetchResource<ReturnType>(id))
	{
		return resource;
	}

	std::unique_ptr<ReturnType> obj    = std::make_unique<ReturnType>(InName, std::forward<Args>(InArgs)...);
	ReturnType*                 rawPtr = obj.get();
	mManagedList.try_emplace(hash, std::move(obj));

	return rawPtr;
}

template <class ManagedType, class Manager>
template <class ReturnType, typename... Args>
ReturnType* Manager_Base<ManagedType, Manager>::CreateOrLoad(const std::string& InName, Args&&... InArgs)
{
	std::string id   = ParseFile(InName);
	uint32_t    hash = StringHash(InName.c_str());

	if (ReturnType* resource = FetchResource<ReturnType>(id))
	{
		return resource;
	}

	std::unique_ptr<ReturnType> obj    = std::make_unique<ReturnType>(InName, std::forward<Args>(InArgs)...);
	ReturnType*                 rawPtr = obj.get();
	mManagedList.try_emplace(hash, std::move(obj));

	return rawPtr;
}

template <class ManagedType, class Manager>
template <class ReturnType>
ReturnType* Manager_Base<ManagedType, Manager>::FetchResource(const std::wstring& InName)
{
	uint32_t hash = StringHash(InName.c_str());
	auto     it   = mManagedList.find(hash);
	if (it != mManagedList.end())
	{
		return dynamic_cast<ReturnType*>(it->second.get());
	}
	return nullptr;
}

template <class ManagedType, class Manager>
template <class ReturnType>
ReturnType* Manager_Base<ManagedType, Manager>::FetchResource(const std::string& InName)
{
	uint32_t hash = StringHash(InName.c_str());
	auto     it   = mManagedList.find(hash);
	if (it != mManagedList.end())
	{
		return dynamic_cast<ReturnType*>(it->second.get());
	}
	return nullptr;
}

template <class ManagedType, class Manager>
void Manager_Base<ManagedType, Manager>::SafeRemove(const std::wstring& InName)
{
	uint32_t hash = StringHash(InName.c_str());
	mManagedList.erase(hash);
}

template <class ManagedType, class Manager>
void Manager_Base<ManagedType, Manager>::SafeRemove(const std::string& InName)
{
	uint32_t hash = StringHash(InName.c_str());
	mManagedList.erase(hash);
}

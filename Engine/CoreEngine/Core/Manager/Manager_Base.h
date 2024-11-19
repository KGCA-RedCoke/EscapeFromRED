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
	Manager_Base()
	{
		InitializeCriticalSection(&mCriticalSection);
	}

	~Manager_Base()
	{
		DeleteCriticalSection(&mCriticalSection);
	}

public:
	/**
	 * Args에 뭘 넣을지 잘 모르겠으면 생성자 확인
	 * @param InName 고유 이름 
	 * @param InArgs 템플릿 클래스 생성자 인자 
	 */
	template <class ReturnType = ManagedType, typename... Args>
	ReturnType* CreateOrLoad(const JWText& InName, Args&&... InArgs);

	/**
	 * Args에 뭘 넣을지 잘 모르겠으면 생성자 확인
	 * @param InName 고유 이름 
	 * @param InArgs 템플릿 클래스 생성자 인자 
	 */
	template <class ReturnType = ManagedType, typename... Args>
	ReturnType* CreateOrLoad(const JText& InName, Args&&... InArgs);

	template <class ReturnType = ManagedType, typename... Args>
	ReturnType* CreateOrClone(const JWText& InName, Args&&... InArgs);
	template <class ReturnType = ManagedType, typename... Args>
	ReturnType* CreateOrClone(const JText& InName, Args&&... InArgs);

	template <class ReturnType = ManagedType>
	ReturnType* FetchResource(const JWText& InName);
	template <class ReturnType = ManagedType>
	ReturnType* FetchResource(const JText& InName);

	void SafeRemove(const JWText& InName);
	void SafeRemove(const JText& InName);

	JArray<ManagedType*> GetManagedList();

	/**
	 * 매니저에서 Create이후 추가 작업이 필요하다면 여기에 구현 (CreateOrLoad, CreateOrClone 후에 자동 호출)
	 */
	virtual void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
								void*        Entity) {};

protected:
	JHash<uint32_t, UPtr<ManagedType>> mManagedList;

private:
	CRITICAL_SECTION mCriticalSection = {};
};

template <class ManagedType, class Manager>
template <class ReturnType, typename... Args>
ReturnType* Manager_Base<ManagedType, Manager>::CreateOrLoad(const std::wstring& InName, Args&&... InArgs)
{
	return CreateOrLoad<ReturnType>(WString2String(InName), std::forward<Args>(InArgs)...);
}

template <class ManagedType, class Manager>
template <class ReturnType, typename... Args>
ReturnType* Manager_Base<ManagedType, Manager>::CreateOrLoad(const std::string& InName, Args&&... InArgs)
{
	std::string id   = ParseFile(InName);
	uint32_t    hash = StringHash(id.c_str());

	EnterCriticalSection(&mCriticalSection);

	if (ReturnType* resource = FetchResource<ReturnType>(id))
	{
		LeaveCriticalSection(&mCriticalSection);

		return resource;
	}

	UPtr<ReturnType> obj = MakeUPtr<ReturnType>(InName, std::forward<Args>(InArgs)...);
	ReturnType*      ptr = obj.get();
	mManagedList.try_emplace(hash, std::move(obj));

	LeaveCriticalSection(&mCriticalSection);

	PostInitialize(InName, id, hash, ptr);

	return ptr;
}

template <class ManagedType, class Manager>
template <class ReturnType, typename... Args>
ReturnType* Manager_Base<ManagedType, Manager>::CreateOrClone(const JWText& InName, Args&&... InArgs)
{
	return CreateOrClone<ReturnType>(WString2String(InName), std::forward<Args>(InArgs)...);
}

template <class ManagedType, class Manager>
template <class ReturnType, typename... Args>
ReturnType* Manager_Base<ManagedType, Manager>::CreateOrClone(const JText& InName, Args&&... InArgs)
{
	JText    id   = ParseFile(InName);
	uint32_t hash = StringHash(id.c_str());

	EnterCriticalSection(&mCriticalSection);

	ReturnType* ptr;

	// 이미 로드된 리소스가 있다면 복사
	if (ReturnType* resource = FetchResource<ReturnType>(id))
	{
		UPtr<class IManagedInterface> copied = resource->Clone();

		UPtr<ReturnType> clone = UPtrCast<ReturnType>(std::move(copied));
		ptr                    = clone.get();

		// mRegisteredClass.try_emplace(StringHash(ptr->GetName()).c_str(), std::move(clone));
		LeaveCriticalSection(&mCriticalSection);

		return ptr;
	}

	// 없으면 팩토리에서 새로 생성
	UPtr<ReturnType> obj = MakeUPtr<ReturnType>(InName, std::forward<Args>(InArgs)...);
	ptr                  = obj.get();
	mManagedList.try_emplace(hash, std::move(obj));

	LeaveCriticalSection(&mCriticalSection);

	PostInitialize(InName, id, hash, ptr);

	return ptr;
}

template <class ManagedType, class Manager>
template <class ReturnType>
ReturnType* Manager_Base<ManagedType, Manager>::FetchResource(const std::wstring& InName)
{
	return FetchResource<ReturnType>(WString2String(InName));
}

template <class ManagedType, class Manager>
template <class ReturnType>
ReturnType* Manager_Base<ManagedType, Manager>::FetchResource(const std::string& InName)
{
	uint32_t hash = StringHash(InName.c_str());
	if (mManagedList.contains(hash))
	{
		return dynamic_cast<ReturnType*>(mManagedList[hash].get());
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

template <class ManagedType, class Manager>
JArray<ManagedType*> Manager_Base<ManagedType, Manager>::GetManagedList()
{
	std::vector<ManagedType*> values;  // 값을 저장할 벡터 생성
	values.reserve(mManagedList.size());     // 미리 할당하여 성능 최적화

	for (const auto& pair : mManagedList)
	{  // unordered_map의 각 요소를 순회
		values.push_back(pair.second.get()); // 값을 벡터에 추가
	}

	return values; // 값의 벡터 반환
}

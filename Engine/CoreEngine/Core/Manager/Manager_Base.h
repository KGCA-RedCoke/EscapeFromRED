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
	Ptr<ReturnType> CreateOrLoad(const JWText& InName, Args&&... InArgs);

	/**
	 * Args에 뭘 넣을지 잘 모르겠으면 생성자 확인
	 * @param InName 고유 이름 
	 * @param InArgs 템플릿 클래스 생성자 인자 
	 */
	template <class ReturnType = ManagedType, typename... Args>
	Ptr<ReturnType> CreateOrLoad(const JText& InName, Args&&... InArgs);

	template <class ReturnType = ManagedType, typename... Args>
	Ptr<ReturnType> CreateOrClone(const JWText& InName, Args&&... InArgs);
	template <class ReturnType = ManagedType, typename... Args>
	Ptr<ReturnType> CreateOrClone(const JText& InName, Args&&... InArgs);

	template <class ReturnType = ManagedType>
	Ptr<ReturnType> FetchResource(const JWText& InName);
	template <class ReturnType = ManagedType>
	Ptr<ReturnType> FetchResource(const JText& InName);

	void SafeRemove(const JWText& InName);
	void SafeRemove(const JText& InName);

	/**
	 * 매니저에서 Create이후 추가 작업이 필요하다면 여기에 구현 (CreateOrLoad, CreateOrClone 후에 자동 호출)
	 */
	virtual void PostInitialize(const JText& ParsedName, const uint32_t NameHash, void* Entity) {}

	JArray<Ptr<ManagedType>> GetManagedList();

protected:
	JHash<uint32_t, Ptr<ManagedType>> mManagedList;

private:
	CRITICAL_SECTION mCriticalSection = {};
};

template <class ManagedType, class Manager>
template <class ReturnType, typename... Args>
Ptr<ReturnType> Manager_Base<ManagedType, Manager>::CreateOrLoad(const std::wstring& InName, Args&&... InArgs)
{
	return CreateOrLoad<ReturnType>(WString2String(InName), std::forward<Args>(InArgs)...);
}

template <class ManagedType, class Manager>
template <class ReturnType, typename... Args>
Ptr<ReturnType> Manager_Base<ManagedType, Manager>::CreateOrLoad(const std::string& InName, Args&&... InArgs)
{
	std::string id   = ParseFile(InName);
	uint32_t    hash = StringHash(id.c_str());

	EnterCriticalSection(&mCriticalSection);

	if (Ptr<ReturnType> resource = FetchResource<ReturnType>(id))
	{
		LeaveCriticalSection(&mCriticalSection);

		return resource;
	}

	Ptr<ReturnType> obj = MakePtr<ReturnType>(InName, std::forward<Args>(InArgs)...);
	mManagedList.try_emplace(hash, obj);

	LeaveCriticalSection(&mCriticalSection);

	PostInitialize(id, hash, obj.get());

	return obj;
}

template <class ManagedType, class Manager>
template <class ReturnType, typename... Args>
Ptr<ReturnType> Manager_Base<ManagedType, Manager>::CreateOrClone(const JWText& InName, Args&&... InArgs)
{
	return CreateOrClone<ReturnType>(WString2String(InName), std::forward<Args>(InArgs)...);
}

template <class ManagedType, class Manager>
template <class ReturnType, typename... Args> Ptr<ReturnType> Manager_Base<ManagedType, Manager>::CreateOrClone(
	const JText& InName, Args&&... InArgs)
{
	JText    id   = ParseFile(InName);
	uint32_t hash = StringHash(id.c_str());

	EnterCriticalSection(&mCriticalSection);

	Ptr<ReturnType> clone = nullptr;

	// 이미 로드된 리소스가 있다면 복사
	if (Ptr<ReturnType> resource = FetchResource<ReturnType>(id))
	{
		// Clone New Object
		clone = std::dynamic_pointer_cast<ReturnType>(resource->Clone());
		return clone;
	}

	// 없으면 팩토리에서 새로 생성
	clone = MakePtr<ReturnType>(InName, std::forward<Args>(InArgs)...);
	mManagedList.try_emplace(hash, clone);

	LeaveCriticalSection(&mCriticalSection);

	PostInitialize(id, hash, clone.get());

	return clone;
}

template <class ManagedType, class Manager>
template <class ReturnType>
Ptr<ReturnType> Manager_Base<ManagedType, Manager>::FetchResource(const std::wstring& InName)
{
	return FetchResource<ReturnType>(WString2String(InName));
}

template <class ManagedType, class Manager>
template <class ReturnType>
Ptr<ReturnType> Manager_Base<ManagedType, Manager>::FetchResource(const std::string& InName)
{
	uint32_t hash = StringHash(InName.c_str());
	if (mManagedList.contains(hash))
	{
		return std::dynamic_pointer_cast<ReturnType>(mManagedList[hash]);
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

template <class ManagedType, class Manager> JArray<Ptr<ManagedType>> Manager_Base<ManagedType, Manager>::GetManagedList()
{
	std::vector<Ptr<ManagedType>> values;  // 값을 저장할 벡터 생성
	values.reserve(mManagedList.size());     // 미리 할당하여 성능 최적화

	for (const auto& pair : mManagedList)
	{  // unordered_map의 각 요소를 순회
		values.push_back(pair.second); // 값을 벡터에 추가
	}

	return values; // 값의 벡터 반환
}

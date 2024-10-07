#pragma once
#include "common_include.h"
#include "Core/Manager/IManagedInterface.h"
#include "Core/Utils/FileIO/JSerialization.h"

enum class EObjectType : uint8_t
{
	BaseObject,
	ActorComponent,
	SceneComponent,
	Level
};

enum EObjectFlags : uint32_t
{
	None          = 0,
	MarkAsDirty   = 1 << 0, // 에셋 변경사항이 있음
	IsVisible     = 1 << 1, // 렌더링 가능한 상태
	IsValid       = 1 << 2, // 업데이트 가능한 상태
	IsPendingKill = 1 << 3, // 소멸 대기 상태
	DontDestroy   = 1 << 4, // 소멸하지 않는 상태 (레벨 전환 시 소멸되지 않음)
};

class JObject : public IManagedInterface, public ISerializable, public std::enable_shared_from_this<JObject>
{
public:
	JObject();
	explicit JObject(JTextView InName);
	~JObject() override = default;

	virtual void Initialize() {}
	virtual void BeginPlay() {}
	virtual void Tick(float DeltaTime) {}

	virtual void Destroy()
	{
		mObjectFlags |= EObjectFlags::IsPendingKill;
	}

	virtual EObjectType GetObjectType() const { return EObjectType::BaseObject; }

	uint32_t GetHash() const override;
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

	template <class ObjectType, typename... Args>
	Ptr<ObjectType> CreateDefaultSubObject(Args... args)
	{
		static_assert(std::is_base_of_v<JObject, ObjectType>, "ObjectType is not derived from JObject");

		Ptr<ObjectType> obj = MakePtr<ObjectType>(std::forward<Args>(args)...);

		mChildObjs.push_back(obj);

		return obj;
	}

	template <class ObjectType>
	Ptr<ObjectType> GetThisPtr()
	{
		return std::dynamic_pointer_cast<ObjectType>(shared_from_this());
	}

	[[nodiscard]] bool IsVisible() const { return mObjectFlags & EObjectFlags::IsVisible; }
	[[nodiscard]] bool IsValid() const { return mObjectFlags & EObjectFlags::IsValid; }
	[[nodiscard]] bool IsPendingKill() const { return mObjectFlags & EObjectFlags::IsPendingKill; }
	[[nodiscard]] bool IsMarkedAsDirty() const { return mObjectFlags & EObjectFlags::MarkAsDirty; }

	void MarkAsDirty() { mObjectFlags |= EObjectFlags::MarkAsDirty; }

	void SetVisible(bool bVisible)
	{
		bVisible ? mObjectFlags |= EObjectFlags::IsVisible : mObjectFlags &= ~EObjectFlags::IsVisible;
	}

	[[nodiscard]] FORCEINLINE JText        GetName() const { return mName; }
	[[nodiscard]] FORCEINLINE Ptr<JObject> GetParentObject() const { return mParentObj.lock(); }
	[[nodiscard]] FORCEINLINE size_t       GetChildCount() const { return mChildObjs.empty() ? mChildObjs.size() : 0; }

protected:
	JText    mName;
	uint32_t mObjectFlags = 0;

	WPtr<JObject>        mParentObj;
	JArray<Ptr<JObject>> mChildObjs;

protected:
	static uint32_t g_DefaultObjectNum;

};

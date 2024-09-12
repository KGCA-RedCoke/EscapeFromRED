#pragma once

#include "common_include.h"

CLASS_PTR(JObject)

class JObject
{
public:
	JObject();
	explicit JObject(JTextView InName);
	virtual  ~JObject() {}

	virtual void Initialize() {}
	virtual void BeginPlay() {}
	virtual void Tick(float DeltaTime) {}

	[[nodiscard]] FORCEINLINE JText GetName() const { return mName; }

	template <class ObjectType, typename... Args>
	ObjectType* CreateDefaultSubObject(Args... args)
	{
		static_assert(std::is_base_of_v<JObject, ObjectType>, "ObjectType is not derived from JObject");

		JObjectUPtr obj    = std::make_unique<ObjectType>(args);
		ObjectType* rawPtr = obj.get();

		mChildObjs.emplace_back(std::move(obj));

		return rawPtr;
	}

	// TODO: 클래스 생성시 CDO 생성
	JObject* GetComponentByID(JTextView InTextView);
	JObject* GetComponentByClass();

protected:
	JText    mName;
	uint32_t mPrimaryKey;

	JObject*                   mParentObj;
	std::vector<UPtr<JObject>> mChildObjs;

	static uint32_t g_DefaultObjectNum;

};

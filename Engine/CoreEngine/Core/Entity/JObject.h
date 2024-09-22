#pragma once

#include "common_include.h"
#include "Core/Utils/FileIO/JSerialization.h"

class JObject : public ISerializable, public std::enable_shared_from_this<JObject>
{
public:
	JObject();
	explicit JObject(JTextView InName);
	~JObject() override = default;

	virtual void Initialize() {}
	virtual void BeginPlay() {}
	virtual void Tick(float DeltaTime) {}

	void Serialize(std::ofstream& FileStream) override;
	void DeSerialize(std::ifstream& InFileStream) override;

	template <class ObjectType, typename... Args>
	ObjectType* CreateDefaultSubObject(Args... args)
	{
		static_assert(std::is_base_of_v<ObjectType, JObject>, "ObjectType is not derived from JObject");

		Ptr<JObject> obj    = std::make_shared<ObjectType>(args);
		ObjectType*  rawPtr = obj.get();

		mChildObjs.emplace_back(std::move(obj));

		return rawPtr;
	}

	[[nodiscard]] FORCEINLINE JText GetName() const { return mName; }
	[[nodiscard]] FORCEINLINE Ptr<JObject> GetParentObject() const { return mParentObj; }
	[[nodiscard]] FORCEINLINE const std::vector<Ptr<JObject>>& GetChildObjects() const { return mChildObjs; }
	[[nodiscard]] FORCEINLINE size_t GetChildCount() const { return mChildObjs.empty() ? mChildObjs.size() : 0; }

protected:
	JText    mName;
	uint32_t mPrimaryKey;

	Ptr<JObject>              mParentObj;
	std::vector<Ptr<JObject>> mChildObjs;

	static uint32_t g_DefaultObjectNum;

};

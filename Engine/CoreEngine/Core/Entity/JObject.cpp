#include "JObject.h"
#include "Core/Utils/Utils.h"


uint32_t JObject::g_DefaultObjectNum = 0;

JObject::JObject()
	: mName(std::format("JObject_{}", g_DefaultObjectNum++))
{}

JObject::JObject(JTextView InName)
	: mName(InName)
{}

Ptr<IManagedInterface> JObject::Clone() const
{
	Ptr<JObject> clone = MakePtr<JObject>(*this);

	clone->mName      = std::format("{}_Clone", mName);
	clone->mParentObj = mParentObj;
	for (const auto& child : mChildObjs)
	{
		clone->mChildObjs.push_back(std::dynamic_pointer_cast<JObject>(child->Clone()));
	}

	return clone;
}

uint32_t JObject::GetHash() const
{
	return StringHash(ParseFile(mName).c_str());
}

uint32_t JObject::GetType() const
{
	return StringHash("JObject");
}

bool JObject::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// Name
	Utils::Serialization::Serialize_Text(mName, FileStream);

	// Child Count
	size_t childCount = mChildObjs.size();
	FileStream.write(reinterpret_cast<char*>(&childCount), sizeof(childCount));

	// Child Objects
	for (int32_t i = 0; i < childCount; ++i)
	{
		mChildObjs[i]->Serialize_Implement(FileStream);
	}

	return true;
}

bool JObject::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// Name
	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// Child Count
	size_t childCount;
	Utils::Serialization::DeSerialize_Primitive(&childCount, sizeof(childCount), InFileStream);

	// Child Objects
	mChildObjs.reserve(childCount);
	for (int32_t i = 0; i < childCount; ++i)
	{
		mChildObjs.emplace_back(MakeUPtr<JObject>());
		mChildObjs[i]->mParentObj = shared_from_this();
		mChildObjs[i]->DeSerialize_Implement(InFileStream);
	}

	return true;
}

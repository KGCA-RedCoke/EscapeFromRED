#include "JObject.h"
#include "Core/Utils/Utils.h"


uint32_t JObject::g_DefaultObjectNum = 0;

JObject::JObject()
	: mName(std::format("JObject_{}", g_DefaultObjectNum++))
{}

JObject::JObject(JTextView InName)
	: mName(ParseFile(InName.data()))
{}

JObject::~JObject() {}

uint32_t JObject::GetHash() const
{
	return StringHash(ParseFile(mName).c_str());
}

UPtr<IManagedInterface> JObject::Clone() const
{
	return nullptr;
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

	// Object Type
	Utils::Serialization::Serialize_Text(mObjectType, FileStream);

	// Object Flags
	Utils::Serialization::Serialize_Primitive(&mObjectFlags, sizeof(mObjectFlags), FileStream);

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

	// Object Type
	Utils::Serialization::DeSerialize_Text(mObjectType, InFileStream);

	// Object Flags
	Utils::Serialization::DeSerialize_Primitive(&mObjectFlags, sizeof(mObjectFlags), InFileStream);

	return true;
}

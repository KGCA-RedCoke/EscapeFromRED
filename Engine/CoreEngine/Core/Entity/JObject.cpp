#include "JObject.h"
#include "Core/Utils/Utils.h"


uint32_t JObject::g_DefaultObjectNum = 0;

JObject::JObject()
	: mName(std::format("JObject_{}", g_DefaultObjectNum++))
{
	mPrimaryKey = StringHash(mName.data());
}

JObject::JObject(JTextView InName)
	: mName(InName)
{
	mPrimaryKey = StringHash(mName.data());
}

void JObject::Serialize(std::ofstream& FileStream)
{
	// Name
	Utils::Serialization::Serialize_Text(mName, FileStream);

	// Primary Key
	mPrimaryKey = StringHash(mName.data());

	// Child Count
	size_t childCount = mChildObjs.size();
	FileStream.write(reinterpret_cast<char*>(&childCount), sizeof(childCount));

	// Child Objects
	for (int32_t i = 0; i < childCount; ++i)
	{
		mChildObjs[i]->Serialize(FileStream);
	}
}

void JObject::DeSerialize(std::ifstream& InFileStream)
{
	// Name
	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// Primary Key
	mPrimaryKey = StringHash(mName.data());

	// Child Count
	size_t childCount;
	InFileStream.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));

	// Child Objects
	mChildObjs.reserve(childCount);
	for (int32_t i = 0; i < childCount; ++i)
	{
		mChildObjs.push_back(std::make_shared<JObject>());
		mChildObjs[i]->mParentObj = shared_from_this();
		mChildObjs[i]->DeSerialize(InFileStream);
	}
}

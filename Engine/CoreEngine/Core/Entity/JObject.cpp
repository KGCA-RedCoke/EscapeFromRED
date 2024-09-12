#include "common_pch.h"
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

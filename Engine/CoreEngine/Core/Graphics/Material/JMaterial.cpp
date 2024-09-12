#include "common_pch.h"
#include "JMaterial.h"

#include "Core/Utils/Utils.h"

JMaterial::JMaterial(JTextView InMaterialName)
	: bTransparent(false)
{
	mMaterialID = StringHash(InMaterialName.data());
}

JMaterial::JMaterial(JWTextView InMaterialName)
	: bTransparent(false)
{
	mMaterialID = StringHash(InMaterialName.data());
}

const FMaterialParams *JMaterial::GetMaterialParam(const JText &InParamName) const
{
	return const_cast<JMaterial *>(this)->GetMaterialParam(InParamName);
}

const FMaterialParams *JMaterial::GetMaterialParam(const JWText &InParamName) const
{
	// const_cast로 상수성을 제거하고 비상수 버전을 호출
	// 코드중복을 사용하지 않기 위해...
	// FIXME: const_cast를 사용하지 않는 방법이 있을까? 
	return const_cast<JMaterial *>(this)->GetMaterialParam(InParamName);
}

FMaterialParams *JMaterial::GetMaterialParam(const JText &InParamName)
{
	if (mMaterialParams.empty())
	{
		return nullptr;
	}

	const uint32_t paramName = StringHash(InParamName.c_str());

	for (auto &param : mMaterialParams)
	{
		if (param.Key == paramName)
		{
			return &param;
		}
	}

	return nullptr;
}

FMaterialParams *JMaterial::GetMaterialParam(const JWText &InParamName)
{
	if (mMaterialParams.empty())
	{
		return nullptr;
	}

	const uint32_t paramName = StringHash(InParamName.c_str());

	for (auto &param : mMaterialParams)
	{
		if (param.Key == paramName)
		{
			return &param;
		}
	}

	return nullptr;
}

void JMaterial::AddMaterialParam(const FMaterialParams &InMaterialParam)
{
	mMaterialParams.push_back(InMaterialParam);
}

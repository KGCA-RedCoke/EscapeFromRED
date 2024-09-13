#include "common_pch.h"
#include "JMaterial.h"

#include "Core/Utils/Utils.h"


JMaterial::JMaterial() {}

JMaterial::JMaterial(JTextView InMaterialName)
	: bTransparent(false)
{
	mMaterialName = {InMaterialName.begin(), InMaterialName.end()};
	mMaterialID   = StringHash(InMaterialName.data());
}

JMaterial::JMaterial(JWTextView InMaterialName)
	: bTransparent(false)
{
	mMaterialName = {InMaterialName.begin(), InMaterialName.end()};
	mMaterialID   = StringHash(InMaterialName.data());
}

void JMaterial::Serialize(std::ofstream& FileStream)
{
	// Material ID
	FileStream.write(reinterpret_cast<const char*>(&mMaterialID), sizeof(mMaterialID));

	// Material Name
	size_t nameSize = mMaterialName.size();
	FileStream.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
	JText rawString = WString2String(mMaterialName);
	FileStream.write(reinterpret_cast<char*>(rawString.data()), nameSize);

	// Transparent
	FileStream.write(reinterpret_cast<char*>(&bTransparent), sizeof(bTransparent));

	// Material Param Count
	int32_t paramCount = mMaterialParams.size();
	FileStream.write(reinterpret_cast<char*>(&paramCount), sizeof(paramCount));

	// Material Params
	for (int32_t i = 0; i < paramCount; ++i)
	{
		mMaterialParams[i].Serialize(FileStream);
	}
}

void JMaterial::DeSerialize(std::ifstream& InFileStream)
{
	// Material ID
	InFileStream.read(reinterpret_cast<char*>(&mMaterialID), sizeof(mMaterialID));

	// Material Name
	size_t nameSize;
	InFileStream.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
	JText rawString(nameSize, '0');
	InFileStream.read(reinterpret_cast<char*>(rawString.data()), nameSize);
	mMaterialName = String2WString(rawString);

	// Transparent
	InFileStream.read(reinterpret_cast<char*>(&bTransparent), sizeof(bTransparent));

	// Material Param Count
	int32_t paramCount;
	InFileStream.read(reinterpret_cast<char*>(&paramCount), sizeof(paramCount));

	// Material Params
	mMaterialParams.reserve(paramCount);
	for (int32_t i = 0; i < paramCount; ++i)
	{
		FMaterialParams param;
		param.DeSerialize(InFileStream);
		mMaterialParams.push_back(param);
	}
}

const FMaterialParams* JMaterial::GetMaterialParam(const JText& InParamName) const
{
	return const_cast<JMaterial*>(this)->GetMaterialParam(InParamName);
}

const FMaterialParams* JMaterial::GetMaterialParam(const JWText& InParamName) const
{
	// const_cast로 상수성을 제거하고 비상수 버전을 호출
	// 코드중복을 사용하지 않기 위해...
	// FIXME: const_cast를 사용하지 않는 방법이 있을까? 
	return const_cast<JMaterial*>(this)->GetMaterialParam(InParamName);
}

FMaterialParams* JMaterial::GetMaterialParam(const JText& InParamName)
{
	if (mMaterialParams.empty())
	{
		return nullptr;
	}

	const uint32_t paramName = StringHash(InParamName.c_str());

	for (auto& param : mMaterialParams)
	{
		if (param.Key == paramName)
		{
			return &param;
		}
	}

	return nullptr;
}

FMaterialParams* JMaterial::GetMaterialParam(const JWText& InParamName)
{
	if (mMaterialParams.empty())
	{
		return nullptr;
	}

	const uint32_t paramName = StringHash(InParamName.c_str());

	for (auto& param : mMaterialParams)
	{
		if (param.Key == paramName)
		{
			return &param;
		}
	}

	return nullptr;
}

void JMaterial::AddMaterialParam(const FMaterialParams& InMaterialParam)
{
	mMaterialParams.push_back(InMaterialParam);
}

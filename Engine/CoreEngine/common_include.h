#pragma once
#include "common_pch.h"

using JText      = std::string;
using JTextView  = std::string_view;
using JWText     = std::wstring;
using JWTextView = std::wstring_view;

template <typename K, typename V>
using JHash = std::unordered_map<K, V>;

template <typename T>
using JArray = std::vector<T>;

template <typename T>
using UPtr = std::unique_ptr<T>;
template <typename T>
using Ptr = std::shared_ptr<T>;
template <typename T>
using WPtr = std::weak_ptr<T>;

#define MakeUPtr std::make_unique
#define MakePtr std::make_shared

#define CONCATENATE(x, y) x##y // 토큰 결합
#define STRINGIFY(x) #x // 문자열화
#define CHECK_PREFIX(name, prefix) static_assert(std::string_view(STRINGIFY(name)).substr(0, 1) == #prefix, "DelegateName must start with '#prefix'");

#define DECLARE_DYNAMIC_DELEGATE(delegateName, ...)\
CHECK_PREFIX(delegateName, F);\
class delegateName {\
public:\
using FunctionType = std::function<void(__VA_ARGS__)>;\
void Bind(FunctionType func) { functions.push_back(func); }\
template<typename... Args>\
void Execute(Args&&... args) { for (auto& func : functions) { func(std::forward<Args>(args)...); } }\
public:\
JArray<FunctionType> functions;\
};

inline JWText String2WString(const JText& InString)
{
	USES_CONVERSION;
	return JWText{A2W(InString.c_str())};
}

inline JText WString2String(const JWText& InWString)
{
	USES_CONVERSION;
	return JText{W2A(InWString.c_str())};
}
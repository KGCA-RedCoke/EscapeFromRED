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

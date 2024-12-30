#pragma once
#include "common_pch.h"

using JText      = std::string;
using JTextView  = std::string_view;
using JWText     = std::wstring;
using JWTextView = std::wstring_view;

template <typename K, typename V>
using JHash = std::unordered_map<K, V>;

template <typename K, typename V>
using JOrderedHash = std::map<K, V>;

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

template <typename Target, typename Source>
UPtr<Target> UPtrCast(UPtr<Source>&& source)
{
	return UPtr<Target>(static_cast<Target*>(source.release()));
}


#define CONCATENATE(x, y) x##y // 토큰 결합
#define STRINGIFY(x) #x // 문자열화
#define CHECK_PREFIX(name, prefix) static_assert(std::string_view(STRINGIFY(name)).substr(0, 1) == #prefix, "DelegateName must start with '#prefix'");

#define DECLARE_DYNAMIC_DELEGATE(delegateName, ...)                            \
CHECK_PREFIX(delegateName, F);                                                \
class delegateName {                                                          \
public:                                                                       \
    using FunctionType = std::function<void(__VA_ARGS__)>;                    \
    using FunctionID = size_t;                                                \
                                                                              \
    FunctionID Bind(FunctionType func) {                                      \
        functions.push_back({++currentID, func});                             \
        return currentID;                                                     \
    }                                                                         \
                                                                              \
    void UnBind(FunctionID id) {                                              \
        auto it = std::remove_if(functions.begin(), functions.end(),          \
            [id](const auto& pair) { return pair.first == id; });             \
        functions.erase(it, functions.end());                                 \
    }                                                                         \
                                                                              \
    void UnBindAll() {                                                        \
    	functions.clear();                                                    \
    }                                                                         \
                                                                              \
    template<typename... Args>                                                \
    void Execute(Args&&... args) {                                            \
        for (auto& [id, func] : functions) {                                  \
            func(std::forward<Args>(args)...);                                \
        }                                                                     \
    }                                                                         \
                                                                              \
private:                                                                      \
    JArray<std::pair<FunctionID, FunctionType>> functions;                    \
    FunctionID currentID = 0;                                                 \
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

#define u8(x) reinterpret_cast<const char*>(u8##x)

inline JText String2UTF8(const JText& Text)
{
	JWText unicodeString{CA2W(Text.c_str())}; // multi_byte -> unicode(utf_16)
	return CW2A(unicodeString.c_str(), CP_UTF8).m_psz; // unicode(utf_16) -> utf_8

}

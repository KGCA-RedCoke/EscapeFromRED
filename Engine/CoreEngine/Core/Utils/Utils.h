#pragma once
#include "common_include.h"

inline JHash<JText, uint32_t> g_HashCache;
inline std::mutex             g_HashCacheMutex;

//--------------------------------------------- String Func -------------------------------------------------------------
inline uint32_t djb2_impl(const char* InString, uint32_t PrevHash)
{
	int32_t c;
	while ((c    = *InString++))
		PrevHash = ((PrevHash << 5) + PrevHash) + c;

	return PrevHash;
}

inline uint32_t StringHash(const char* InString)
{
	JText value = InString;

	{
		// 뮤텍스를 사용해 스레드 안전하게 접근.
		std::lock_guard<std::mutex> lock(g_HashCacheMutex);
		auto                        it = g_HashCache.find(value);
		if (it != g_HashCache.end())
		{
			// 캐싱된 값을 찾으면 반환.
			return it->second;
		}
	}

	uint32_t hash = djb2_impl(InString, 5381);

	{
		// 계산된 해시 값을 캐시에 저장.
		std::lock_guard<std::mutex> lock(g_HashCacheMutex);
		g_HashCache.emplace(std::move(value), hash);
	}

	return hash;
}

inline uint32_t StringHash(const wchar_t* InString)
{
	return djb2_impl(WString2String(InString).data(), 5381);
}

//--------------------------------------------- String Func -------------------------------------------------------------


// ------------------------------------------- JHash Table ---------------------------------------------------------------
const uint32_t JAssetHash = StringHash("JASSET\0");

const uint32_t HASH_ASSET_TYPE_Actor             = StringHash("JActor");
const uint32_t HASH_ASSET_TYPE_STATIC_MESH       = StringHash("J3DObject");
const uint32_t HASH_ASSET_TYPE_SKELETAL_MESH     = StringHash("JSkeletalMesh");
const uint32_t HASH_ASSET_TYPE_MATERIAL          = StringHash("JMaterial");
const uint32_t HASH_ASSET_TYPE_MATERIAL_INSTANCE = StringHash("JMaterialInstance");
const uint32_t HASH_ASSET_TYPE_ANIMATION_CLIP    = StringHash("JAnimationClip");

const uint32_t Hash_EXT_FBX    = StringHash(".fbx");
const uint32_t Hash_EXT_JASSET = StringHash(".jasset");

const uint32_t HASH_EXT_PNG = StringHash(".png");
const uint32_t HASH_EXT_JPG = StringHash(".jpg");
const uint32_t HASH_EXT_TGA = StringHash(".tga");
const uint32_t HASH_EXT_BMP = StringHash(".bmp");
const uint32_t HASH_EXT_DDS = StringHash(".dds");

const uint32_t HASH_EXT_HLSL = StringHash(".hlsl");
//---------------------------------------------- String Func --------------------------------------------------------------


//------------------------------------------------- Enum  -----------------------------------------------------------------
template <typename EnumType>
constexpr uint8_t EnumAsByte(EnumType Type)
{
	return static_cast<uint8_t>(Type);
}

//------------------------------------------------- Enum  -----------------------------------------------------------------

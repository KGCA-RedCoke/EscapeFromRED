#pragma once
#include <regex>

#include "common_include.h"
#include "Math/MathUtility.h"

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

constexpr uint32_t djb2_imple_CompileTime(const char* InString, uint32_t PrevHash)
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

constexpr uint32_t StringHash_CompileTime(const char* InString)
{
	return djb2_imple_CompileTime(InString, 5381);
}

inline JText GenerateUniqueFileName(const JText& InDirectory, const JText& InBaseName, const JText& InExtension)
{
	namespace fs = std::filesystem;

	int        maxIndex = 0;
	std::regex filePattern(InBaseName + R"((\d+)?)" + InExtension);

	// 같은 경로 내에서 순회 (같은 이름을 가진 파일이 있는지 확인)
	for (const auto& entry : fs::directory_iterator(InDirectory))
	{
		if (entry.is_regular_file())
		{
			std::smatch match;
			std::string filename = entry.path().filename().string();
			if (std::regex_match(filename, match, filePattern))
			{
				if (match.size() > 1 && match[1].matched)
				{
					maxIndex = FMath::Max(maxIndex, std::stoi(match[1].str()));
				}
			}
		}
	}

	// 새 파일 이름 생성
	return std::format("{}/{}{}{}", InDirectory, InBaseName, maxIndex + 1, InExtension);
}

//--------------------------------------------- String Func -------------------------------------------------------------


// ------------------------------------------- JHash Table ---------------------------------------------------------------
constexpr const char* NAME_OBJECT_BASE                    = "JObject";
constexpr const char* NAME_OBJECT_ACTOR_COMPONENT         = "JActorComponent";
constexpr const char* NAME_OBJECT_SCENE_COMPONENT         = "JSceneComponent";
constexpr const char* NAME_OBJECT_STATIC_MESH_COMPONENT   = "JStaticMeshComponent";
constexpr const char* NAME_OBJECT_SKELETAL_MESH_COMPONENT = "JSkeletalMeshComponent";
constexpr const char* NAME_OBJECT_ACTOR                   = "AActor";
constexpr const char* NAME_OBJECT_STATIC_MESH_ACTOR       = "JStaticMeshActor";
constexpr const char* NAME_OBJECT_SKELETAL_MESH_ACTOR     = "JSkeletalMeshActor";
constexpr const char* NAME_OBJECT_LIGHT_COMPONENT         = "JLightComponent";
constexpr const char* NAME_OBJECT_LIGHT_SPOT              = "JLightSpot";
constexpr const char* NAME_OBJECT_LIGHT_POINT             = "JLightPoint";
constexpr const char* NAME_OBJECT_LEVEL                   = "JLevel";


constexpr uint32_t JAssetHash = StringHash_CompileTime("JASSET\0");

constexpr uint32_t HASH_ASSET_TYPE_LEVEL             = StringHash_CompileTime("JLevel");
constexpr uint32_t HASH_ASSET_TYPE_Actor             = StringHash_CompileTime("JActor");
constexpr uint32_t HASH_ASSET_TYPE_STATIC_MESH       = StringHash_CompileTime("J3DObject");
constexpr uint32_t HASH_ASSET_TYPE_SKELETAL_MESH     = StringHash_CompileTime("JSkeletalMesh");
constexpr uint32_t HASH_ASSET_TYPE_MATERIAL          = StringHash_CompileTime("JMaterial");
constexpr uint32_t HASH_ASSET_TYPE_MATERIAL_INSTANCE = StringHash_CompileTime("JMaterialInstance");
constexpr uint32_t HASH_ASSET_TYPE_ANIMATION_CLIP    = StringHash_CompileTime("JAnimationClip");

constexpr uint32_t Hash_EXT_FBX    = StringHash_CompileTime(".fbx");
constexpr uint32_t Hash_EXT_JASSET = StringHash_CompileTime(".jasset");

constexpr uint32_t HASH_EXT_PNG = StringHash_CompileTime(".png");
constexpr uint32_t HASH_EXT_JPG = StringHash_CompileTime(".jpg");
constexpr uint32_t HASH_EXT_TGA = StringHash_CompileTime(".tga");
constexpr uint32_t HASH_EXT_BMP = StringHash_CompileTime(".bmp");
constexpr uint32_t HASH_EXT_DDS = StringHash_CompileTime(".dds");

constexpr uint32_t HASH_EXT_WAV = StringHash_CompileTime(".wav");
constexpr uint32_t HASH_EXT_MP3 = StringHash_CompileTime(".mp3");

constexpr uint32_t HASH_EXT_HLSL = StringHash_CompileTime(".hlsl");
//---------------------------------------------- String Func --------------------------------------------------------------


//------------------------------------------------- Enum  -----------------------------------------------------------------
template <typename EnumType>
constexpr uint8_t EnumAsByte(EnumType Type)
{
	return static_cast<uint8_t>(Type);
}

//------------------------------------------------- Enum  -----------------------------------------------------------------

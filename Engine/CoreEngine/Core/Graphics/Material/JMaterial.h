#pragma once
#include "common_include.h"
#include "Core/Utils/Utils.h"
#include "Core/Utils/Math/Vector4.h"

class JTexture;

/**
 * @brief Material parameter type
 */
enum class EMaterialParamType : uint8_t
{
	Boolean,
	Integer,
	Float,
	Float2,
	Float3,
	Float4,
	String,
	Texture2D,
	TextureCube,
	TextureVolume
};

/**
 * @brief Material flag
 */
enum class EMaterialFlag : uint32_t
{
	None     = 0,
	Diffuse  = 1 << 0, // 발광 색상
	Specular = 1 << 1, // 반사 색상
	Ambient  = 1 << 2, // 주변 색상
	Emissive = 1 << 3, // 방출 색상
	Normal   = 1 << 4, // 노말맵
	Opacity  = 1 << 5, // 투명도
	Alpha    = 1 << 6, // 알파 블렌딩
	Bump     = 1 << 7, // 범프맵
};

/**
 * @brief Material parameters
 * @details 머티리얼 템플릿 변수
 * ScalarParameter, VectorParameter, TextureParameter를 저장하는 구조체
 * Diffuse, Specular, Ambient, Emissive, Normal, Opacity 등
 */
struct FMaterialParams
{
	JText              Name; /** 머티리얼 변수 이름 */
	bool               bInstanceParam = false; /** 인스턴스 변수 여부 */
	uint32_t           Key            = 0; /** 머티리얼 변수 해시값 (빠른 비교를 위해 사용) */
	EMaterialParamType ParamType      = EMaterialParamType::String; /** 머티리얼 변수 타입 */
	EMaterialFlag      Flags          = EMaterialFlag::Diffuse; /** 머티리얼 변수 플래그 */
	JTexture*          TextureValue   = nullptr;

	union
	{
		bool        BooleanValue = false;
		int32_t     IntegerValue;
		float       FloatValue;
		FVector2    Float2Value;
		FVector     Float3Value;
		FVector4    Float4Value;
		const char* StringValue;
	};
};

/**
 * @brief Material class
 */
class JMaterial
{
public:
	JMaterial(JTextView InMaterialName);
	JMaterial(JWTextView InMaterialName);
	~JMaterial() = default;

public:
	[[nodiscard]] const FMaterialParams*              GetMaterialParam(const JText& InParamName) const;
	[[nodiscard]] const FMaterialParams*              GetMaterialParam(const JWText& InParamName) const;
	[[nodiscard]] FMaterialParams*                    GetMaterialParam(const JText& InParamName);
	[[nodiscard]] FMaterialParams*                    GetMaterialParam(const JWText& InParamName);
	[[nodiscard]] const std::vector<FMaterialParams>& GetMaterialParams() const { return mMaterialParams; }
	[[nodiscard]] uint32_t                            GetMaterialID() const { return mMaterialID; }
	[[nodiscard]] bool                                HasMaterial() const { return !mMaterialParams.empty(); }
	void                                              AddMaterialParam(const FMaterialParams& InMaterialParam);

	FORCEINLINE void               SetTransparent(bool bInTransparent) { bTransparent = bInTransparent; }
	[[nodiscard]] FORCEINLINE bool IsTransparent() const { return bTransparent; }
	
private:
	uint32_t mMaterialID;

	bool bTransparent;

	std::vector<FMaterialParams> mMaterialParams;
};

namespace Utils::Material
{
	/**
	 * 머티리얼에 텍스처 파라미터를 추가
	 * @param ParamName 변수 이름
	 * @param Index 변수 인덱스
	 * @param FileName 텍스처 파일 경로
	 * @param Flag 텍스처 파일 플래그(종류)
	 */
	inline FMaterialParams CreateTextureParam(const char*   ParamName, const char* FileName, int32_t Index,
											  EMaterialFlag Flag)
	{
		FMaterialParams materialParams;

		if (Index == 0)
		{
			materialParams.Name.assign(ParamName);
		}
		else
		{
			materialParams.Name = std::format("{0}_{1}", ParamName, Index);
		}

		materialParams.Key            = StringHash(materialParams.Name.c_str());
		materialParams.ParamType      = EMaterialParamType::Texture2D;
		materialParams.StringValue    = FileName;
		materialParams.bInstanceParam = true;
		materialParams.Flags          = Flag;

		return materialParams;
	}



	


}

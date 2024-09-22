#pragma once
#include "common_include.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Utils/Utils.h"
#include "Core/Utils/FileIO/JSerialization.h"
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
struct FMaterialParams : public ISerializable
{
	JText              Name; /** 머티리얼 변수 이름 */
	bool               bInstanceParam = false; /** 인스턴스 변수 여부 */
	uint32_t           Key            = 0; /** 머티리얼 변수 해시값 (빠른 비교를 위해 사용) */
	EMaterialParamType ParamType      = EMaterialParamType::String; /** 머티리얼 변수 타입 */
	EMaterialFlag      Flags          = EMaterialFlag::Diffuse; /** 머티리얼 변수 플래그 */
	JTexture*          TextureValue   = nullptr;
	JText              StringValue;

	union
	{
		bool     BooleanValue = false;
		int32_t  IntegerValue;
		float    FloatValue;
		FVector2 Float2Value;
		FVector  Float3Value;
		FVector4 Float4Value;
	};

	void Serialize(std::ofstream& FileStream) override
	{
		// Name
		Utils::Serialization::Serialize_Text(Name, FileStream);

		// bInstanceParam
		Utils::Serialization::Serialize_Primitive(&bInstanceParam, sizeof(bInstanceParam), FileStream);

		// Key
		Utils::Serialization::Serialize_Primitive(&Key, sizeof(Key), FileStream);

		// ParamType
		Utils::Serialization::Serialize_Primitive(&ParamType, sizeof(ParamType), FileStream);

		// Flags
		Utils::Serialization::Serialize_Primitive(&Flags, sizeof(Flags), FileStream);

		switch (ParamType)
		{
		case EMaterialParamType::Boolean:
			Utils::Serialization::Serialize_Primitive(&BooleanValue, sizeof(BooleanValue), FileStream);
			break;
		case EMaterialParamType::Integer:
			Utils::Serialization::Serialize_Primitive(&IntegerValue, sizeof(IntegerValue), FileStream);
			break;
		case EMaterialParamType::String:
		case EMaterialParamType::Texture2D:
			{
				Utils::Serialization::Serialize_Text(StringValue, FileStream);
			}
		default:
			break;
		}
	}

	void DeSerialize(std::ifstream& InFileStream) override
	{
		// Name
		Utils::Serialization::DeSerialize_Text(Name, InFileStream);

		// bInstanceParam
		Utils::Serialization::DeSerialize_Primitive(&bInstanceParam, sizeof(bInstanceParam), InFileStream);

		// Key
		Utils::Serialization::DeSerialize_Primitive(&Key, sizeof(Key), InFileStream);

		// ParamType
		Utils::Serialization::DeSerialize_Primitive(&ParamType, sizeof(ParamType), InFileStream);

		// Flags
		Utils::Serialization::DeSerialize_Primitive(&Flags, sizeof(Flags), InFileStream);

		// Value
		switch (ParamType)
		{
		case EMaterialParamType::Boolean:
			Utils::Serialization::DeSerialize_Primitive(&BooleanValue, sizeof(BooleanValue), InFileStream);
			break;
		case EMaterialParamType::Integer:
			Utils::Serialization::DeSerialize_Primitive(&IntegerValue, sizeof(IntegerValue), InFileStream);
			break;
		case EMaterialParamType::String:
		case EMaterialParamType::Texture2D:
			{
				Utils::Serialization::DeSerialize_Text(StringValue, InFileStream);
			}

			break;
		default:
			break;
		}
	}
};

/**
 * @brief Material class
 */
class JMaterial : public ISerializable
{
public:
	JMaterial();
	explicit JMaterial(JTextView InMaterialName);
	explicit JMaterial(JWTextView InMaterialName);
	~JMaterial() override = default;

public:
	void Serialize(std::ofstream& FileStream) override;
	void DeSerialize(std::ifstream& InFileStream) override;

public:
	/**
	 * 셰이더, (머티리얼) 활성화
	 */
	void BindMaterial(ID3D11DeviceContext* InDeviceContext);
	/**
	 * 필요한 파라미터(텍스처, 색상 등)를 추가
	 * @param InMaterialParam 추가할 파라미터
	 */
	void AddMaterialParam(const FMaterialParams& InMaterialParam);

public:
	[[nodiscard]] const FMaterialParams* GetMaterialParam(const JText& InParamName) const;
	[[nodiscard]] const FMaterialParams* GetMaterialParam(const JWText& InParamName) const;
	[[nodiscard]] FMaterialParams*       GetMaterialParam(const JText& InParamName);
	[[nodiscard]] FMaterialParams*       GetMaterialParam(const JWText& InParamName);

	[[nodiscard]] FORCEINLINE const std::vector<FMaterialParams>& GetMaterialParams() const { return mMaterialParams; }
	[[nodiscard]] FORCEINLINE const JWText&                       GetMaterialName() const { return mMaterialName; }
	[[nodiscard]] FORCEINLINE bool                                HasMaterial() const { return !mMaterialParams.empty(); }
	[[nodiscard]] FORCEINLINE bool                                IsTransparent() const { return bTransparent; }

	FORCEINLINE void SetShader(class JShader* InShader) { mShader = InShader; }
	FORCEINLINE void SetTransparent(bool bInTransparent) { bTransparent = bInTransparent; }

private:
	void ApplyTextures();

private:
	JWText                       mMaterialName;
	uint32_t                     mMaterialID;
	std::vector<FMaterialParams> mMaterialParams;

	class JShader* mShader; // 머티리얼 내에서 셰이더를 가지는게 자연스럽다

	bool bTransparent;
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
	FMaterialParams CreateTextureParam(const char*   ParamName, const char* FileName, int32_t Index,
									   EMaterialFlag Flag);


}

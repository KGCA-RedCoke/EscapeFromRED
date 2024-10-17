#pragma once
#include "common_include.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Utils/Utils.h"
#include "Core/Utils/FileIO/JSerialization.h"
#include "Core/Utils/Math/Vector4.h"


class JShader;
class JTexture;

constexpr auto NAME_MAT_Diffuse       = "DiffuseColor";
constexpr auto NAME_MAT_DiffuseF      = "DiffuseFactor";
constexpr auto NAME_MAT_Emissive      = "EmissiveColor";
constexpr auto NAME_MAT_EmissiveF     = "EmissiveFactor";
constexpr auto NAME_MAT_Specular      = "SpecularColor";
constexpr auto NAME_MAT_SpecularF     = "SpecularFactor";
constexpr auto NAME_MAT_Reflection    = "ReflectionColor";
constexpr auto NAME_MAT_ReflectionF   = "ReflectionFactor";
constexpr auto NAME_MAT_Ambient       = "AmbientColor";
constexpr auto NAME_MAT_AmbientF      = "AmbientFactor";
constexpr auto NAME_MAT_Normal        = "NormalMap";
constexpr auto NAME_MAT_Transparent   = "TransparentColor";
constexpr auto NAME_MAT_TransparentF  = "TransparentFactor";
constexpr auto NAME_MAT_Displacement  = "DisplacementColor";
constexpr auto NAME_MAT_DisplacementF = "DisplacementFactor";
constexpr auto NAME_MAT_Shininess     = "Shininess";

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
enum class EMaterialFlag : uint8_t
{
	DiffuseColor,		// 기본 색상
	EmissiveColor,		// 방출 색상
	SpecularColor,		// 반사 색상 (하이라이트)
	ReflectionColor,	// 반사
	AmbientColor,		// 주변 색상
	NormalMap,			// 노말맵
	DisplacementColor,	// 변위
	TransparentColor,	// 투명도

	DiffuseFactor,		// 기본 색상 팩터
	EmissiveFactor, 	// 방출 색상 팩터
	SpecularFactor, 	// 반사 색상 팩터
	ReflectionFactor,	// 반사 팩터
	AmbientFactor,		// 주변 색상 팩터
	TransparentFactor,	// 투명도 팩터
	DisplacementFactor, // 변위 팩터
	Shininess,			// 광택
	Opacity,			// 투명도

	// BumpMap,		// 범프맵 (노말맵으로 대체)
	// BumpFactor,	// 범프맵 팩터
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
	EMaterialFlag      Flags          = EMaterialFlag::DiffuseColor; /** 머티리얼 변수 플래그 */
	Ptr<JTexture>      TextureValue   = nullptr;
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

	uint32_t GetType() const override
	{
		return StringHash("FMaterialParams");
	}

	bool Serialize_Implement(std::ofstream& FileStream) override
	{
		if (!Utils::Serialization::SerializeMetaData(FileStream, this))
		{
			return false;
		}

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
		case EMaterialParamType::Float:
			Utils::Serialization::Serialize_Primitive(&FloatValue, sizeof(FloatValue), FileStream);
			break;
		case EMaterialParamType::Float2:
			Utils::Serialization::Serialize_Primitive(&Float2Value, sizeof(Float2Value), FileStream);
			break;
		case EMaterialParamType::Float3:
			Utils::Serialization::Serialize_Primitive(&Float3Value, sizeof(Float3Value), FileStream);
			break;
		case EMaterialParamType::String:
		case EMaterialParamType::Texture2D:
			{
				Utils::Serialization::Serialize_Text(StringValue, FileStream);
			}
		default:
			break;
		}

		return true;
	}

	bool DeSerialize_Implement(std::ifstream& InFileStream) override
	{
		JAssetMetaData metaData;
		if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
		{
			return false;
		}

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
		case EMaterialParamType::Float:
			Utils::Serialization::DeSerialize_Primitive(&FloatValue, sizeof(FloatValue), InFileStream);
			break;
		case EMaterialParamType::Float2:
			Utils::Serialization::DeSerialize_Primitive(&Float2Value, sizeof(Float2Value), InFileStream);
			break;
		case EMaterialParamType::Float3:
			Utils::Serialization::DeSerialize_Primitive(&Float3Value, sizeof(Float3Value), InFileStream);
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

		return true;
	}
};

/**
 * @brief Material class
 */
class JMaterial : public IManagedInterface, public ISerializable
{
public:
	JMaterial();
	explicit JMaterial(JTextView InMaterialName);
	explicit JMaterial(JWTextView InMaterialName);
	~JMaterial() override = default;

public:
	uint32_t GetHash() const override;
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	/**
	 * 셰이더, (머티리얼) 활성화
	 */
	void ApplyMaterialParams(ID3D11DeviceContext* InDeviceContext);

	template <typename Value>
	void AddMaterialParam(const JText& InParamName, const EMaterialParamType InParamType, const EMaterialFlag InFlags,
						  Value        InValue)
	{
		FMaterialParams param;
		param.Name      = InParamName;
		param.Key       = StringHash(InParamName.c_str());
		param.ParamType = InParamType;
		param.Flags     = InFlags;

		param.BooleanValue = false;

		if constexpr (std::is_same_v<Value, bool>)
		{
			param.BooleanValue = InValue;
		}
		else if constexpr (std::is_same_v<Value, int32_t>)
		{
			param.IntegerValue = InValue;
		}
		else if constexpr (std::is_same_v<Value, float>)
		{
			param.FloatValue = InValue;
		}
		else if constexpr (std::is_same_v<Value, FVector2>)
		{
			param.Float2Value = InValue;
		}
		else if constexpr (std::is_same_v<Value, FVector>)
		{
			param.Float3Value = InValue;
		}
		else if constexpr (std::is_same_v<Value, FVector4>)
		{
			param.Float4Value = InValue;
		}
		else if constexpr (std::is_same_v<Value, JText>)
		{
			param.StringValue = InValue;
		}
		else
		{
			// 지원하지 않는 타입에 대한 예외처리
		}

		mMaterialParams.push_back(param);
	}

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
	[[nodiscard]] FORCEINLINE std::vector<FMaterialParams>&       GetMaterialParams() { return mMaterialParams; }
	[[nodiscard]] FORCEINLINE uint32_t                            GetMaterialID() const { return mMaterialID; }
	[[nodiscard]] FORCEINLINE ComPtr<ID3D11Buffer>                GetMaterialBuffer() const { return mMaterialBuffer; }
	[[nodiscard]] FORCEINLINE const CBuffer::Material&            GetMaterial() const { return mMaterial; }
	[[nodiscard]] FORCEINLINE const JText&                        GetMaterialPath() const { return mMaterialPath; }
	[[nodiscard]] FORCEINLINE const JText&                        GetMaterialName() const { return mMaterialName; }
	[[nodiscard]] FORCEINLINE bool                                HasMaterial() const { return !mMaterialParams.empty(); }
	[[nodiscard]] FORCEINLINE bool                                IsTransparent() const { return bTransparent; }

	[[nodiscard]] FORCEINLINE Ptr<JShader> GetShader() const { return mShader; }

	FORCEINLINE void SetShader(const Ptr<JShader>& InShader) { mShader = InShader; }
	FORCEINLINE void SetTransparent(bool bInTransparent) { bTransparent = bInTransparent; }

private:
	void ApplyTextures();

private:
	JText                        mMaterialPath;
	JText                        mMaterialName;
	uint32_t                     mMaterialID;
	std::vector<FMaterialParams> mMaterialParams;

	Ptr<JShader> mShader; // 머티리얼 내에서 셰이더를 가지는게 자연스럽다

	ComPtr<ID3D11Buffer> mMaterialBuffer;
	CBuffer::Material    mMaterial;

	bool bTransparent;

	friend class GUI_Editor_Material;
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

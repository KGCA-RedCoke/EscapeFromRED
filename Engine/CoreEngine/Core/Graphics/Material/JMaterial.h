#pragma once
#include "common_include.h"
#include "Core/Graphics/Shader/FConstantBuffer.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Utils/FileIO/JSerialization.h"
#include "Core/Utils/Math/Vector4.h"


class JShader;
class JTexture;

constexpr const char* NAME_MAT_Diffuse       = "Diffuse";
constexpr const char* NAME_MAT_DiffuseF      = "DiffuseFactor";
constexpr const char* NAME_MAT_Emissive      = "Emissive";
constexpr const char* NAME_MAT_EmissiveF     = "EmissiveFactor";
constexpr const char* NAME_MAT_Specular      = "Specular";
constexpr const char* NAME_MAT_SpecularF     = "SpecularFactor";
constexpr const char* NAME_MAT_Reflection    = "Reflection";
constexpr const char* NAME_MAT_ReflectionF   = "ReflectionFactor";
constexpr const char* NAME_MAT_Ambient       = "Ambient";
constexpr const char* NAME_MAT_AmbientF      = "AmbientFactor";
constexpr const char* NAME_MAT_Normal        = "NormalMap";
constexpr const char* NAME_MAT_Transparent   = "Transparent";
constexpr const char* NAME_MAT_TransparentF  = "TransparentFactor";
constexpr const char* NAME_MAT_Displacement  = "Displacement";
constexpr const char* NAME_MAT_DisplacementF = "DisplacementFactor";
constexpr const char* NAME_MAT_Shininess     = "Shininess";

constexpr const char* NAME_MAT_INS_DEFAULT = "Game/Materials/DefaultMaterial.jasset";
constexpr const char* NAME_MAT_BASIC       = "Game/Materials/Engine/Material_Basic.jasset";
constexpr const char* NAME_MAT_SKYSPHERE   = "Game/Materials/Engine/Material_SkySphere.jasset";
constexpr const char* NAME_MAT_WINDOW      = "Game/Materials/Engine/Material_Window.jasset";
constexpr const char* NAME_MAT_POM         = "Game/Materials/Engine/Material_POM.jasset";
constexpr const char* NAME_MAT_DETAIL      = "Game/Materials/Engine/Material_Detail.jasset";
constexpr const char* NAME_MAT_MIRROR      = "Game/Materials/Engine/Material_Mirror.jasset";
constexpr const char* NAME_MAT_2D          = "Game/Materials/Engine/Material_2D.jasset";
constexpr const char* NAME_MAT_LANDSCAPE   = "Game/Materials/Engine/Material_Landscape.jasset";

/**
 * @brief Material parameter type
 */
enum class EMaterialParamValue : uint8_t
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
	TextureVolume,

	Max
};

constexpr const char* HASH_MATERIAL_PARAM_VALUE_TYPE[] = {
	"Boolean",
	"Integer",
	"Float",
	"Float2",
	"Float3",
	"Float4",
	"String",
	"Texture2D",
	/*"TextureCube",
	"TextureVolume"*/
};

/**
 * @brief Material parameters
 * @details 머티리얼 템플릿 변수
 * ScalarParameter, VectorParameter, TextureParameter를 저장하는 구조체
 * Diffuse, Specular, Ambient, Emissive, Normal, Opacity 등
 */
struct FMaterialParam : public ISerializable
{
	JText    Name;						/** 변수 이름 */
	uint32_t Key;						/** 변수 키 */
	bool     bInstanceParam = false;	/** 인스턴스 변수 여부 */

	EMaterialParamValue ParamValue = EMaterialParamValue::String; /** 머티리얼 변수 값 타입 */

	JTexture* TextureValue = nullptr;
	JText     StringValue;

	union
	{
		bool     BooleanValue = false;
		int32_t  IntegerValue;
		float    FloatValue;
		FVector2 Float2Value;
		FVector  Float3Value;
		FVector4 Float4Value;
	};

	FMaterialParam() = default;
	FMaterialParam(const JText& InName);
	FMaterialParam(const JText& InName, EMaterialParamValue InParamValue, const void* InValue,
				   bool         bInInstanceParam = false);

	bool operator==(const FMaterialParam& Other) const
	{
		return Key == Other.Key;
	}

	uint32_t GetType() const override;


	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

	void BindMaterialParam(ID3D11DeviceContext* InDeviceContext, uint32_t Index) const;
};

/**
 * @brief Material class
 */
class JMaterial : public JAsset
{
public:
	explicit JMaterial(JTextView InMaterialName);
	explicit JMaterial(JWTextView InMaterialName);
	~JMaterial() override = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

protected:
	virtual void InitializeParams() {};

public:
	/**
	 * 머티리얼을 GPU에 바인딩
	 */
	virtual void BindShader(ID3D11DeviceContext* InDeviceContext);
	void UpdateMaterialParams(ID3D11DeviceContext* InDeviceContext, const void* InData, uint32_t InSize);

	void EditMaterialParam(const JText& InParamName, const FMaterialParam& InMaterialParam);

public:
	[[nodiscard]] const FMaterialParam* GetMaterialParam(const JText& InParamName) const;
	[[nodiscard]] FMaterialParam*       GetMaterialParam(const JText& InParamName);

	[[nodiscard]] FORCEINLINE const JArray<FMaterialParam>& GetMaterialParams() const { return mMaterialParams; }
	[[nodiscard]] FORCEINLINE JArray<FMaterialParam>&       GetMaterialParams() { return mMaterialParams; }
	[[nodiscard]] FORCEINLINE const JText&                  GetMaterialPath() const { return mMaterialPath; }
	[[nodiscard]] FORCEINLINE const JText&                  GetMaterialName() const { return mMaterialName; }
	[[nodiscard]] FORCEINLINE bool                          HasMaterial() const { return !mMaterialParams.empty(); }
	[[nodiscard]] FORCEINLINE JShader*                      GetShader() const { return mShader; }

	FORCEINLINE void SetShader(JShader* InShader);

protected:
	JText    mMaterialPath;
	JText    mMaterialName;
	uint32_t mMaterialID;

	JArray<FMaterialParam> mMaterialParams;
	JShader*               mShader; // 머티리얼 내에서 셰이더를 가지는게 자연스럽다

	friend class GUI_Editor_Material;
	friend class JMaterialInstance;
};

namespace Utils::Material
{
	/**
	 * 머티리얼에 텍스처 파라미터를 추가
	 * @param ParamName 변수 이름
	 * @param Index 변수 인덱스
	 * @param FileName 텍스처 파일 경로
	 */
	FMaterialParam CreateTextureParam(const char* ParamName, const char* FileName, int32_t Index);
}

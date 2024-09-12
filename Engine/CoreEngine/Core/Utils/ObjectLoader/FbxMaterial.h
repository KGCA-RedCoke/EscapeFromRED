#pragma once
#include "common_include.h"
#include "Core/Utils/Math/Vector4.h"

enum class EMaterialExportParamType : uint8_t
{
	String,
	Bool,
	Integer,
	Float,
	Float2,
	Float3,
	Float4,
	Texture2D,
	TextureCube,
	TextureVolume
};

enum class EMaterialExportParamFlag : uint8_t
{
	None         = 0,
	BumpMap      = 1 << 0,
	DiffuseMap   = 1 << 1,
	NormalMap    = 1 << 2,
	SpecularMap  = 1 << 3,
	AlphaChannel = 1 << 4,
};

struct FMaterialExportAttribute
{
	enum EAttributeType
	{
		UnKnown = 0,
		String,
		Int,
		Float,
		Vec4
	};

	JText          Name;
	EAttributeType Type;

	union
	{
		int32_t  IntegerValue;
		float    FloatValue;
		FVector4 Vec4Value;
	};
};

struct FMaterialExportParams
{
	JText                    Name;
	JText                    Hint;
	bool                     bInstanceParam;
	EMaterialExportParamType ParamType;
	EMaterialExportParamFlag Flags;

	union
	{
		const char* StringValue;
		float       FloatValue[16];
		int32_t     IntValue;
	};

	FMaterialExportParams()  = default;
	~FMaterialExportParams() = default;
};


class CFbxMaterial
{
public:
	CFbxMaterial() = default;

	CFbxMaterial(JText InName)
		: mName(InName) {}

	~CFbxMaterial() = default;

public:
	FORCEINLINE void AddParam(const FMaterialExportParams& InParams) { mParams.push_back(InParams); }

	FORCEINLINE bool IsTransparent() const { return bTransparent; }
	FORCEINLINE void SetTransparent(bool bEnableTransparent) { bTransparent = bEnableTransparent; }

public:
	JText mName;

	bool bTransparent = false;

	std::list<FMaterialExportAttribute*> mAttributes;
	std::vector<FMaterialExportParams>   mParams;
};

using FbxMaterialList = std::vector<std::vector<CFbxMaterial*>>;

#include "JMaterial_Wind.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Shader/JShader.h"
#include "Core/Interface/JWorld.h"
#include "GUI/MGUIManager.h"

JMaterial_Wind::JMaterial_Wind(JTextView InName)
	: JMaterial(InName)
{
	JMaterial_Wind::InitializeParams();
}

void JMaterial_Wind::BindShader(ID3D11DeviceContext* InDeviceContext)
{
	// 상수버퍼, 셰이더 세팅
	JMaterial::BindShader(InDeviceContext);

	G_Device.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int32_t slots = 0;
	G_Device.SetSamplerState(ESamplerState::LinearWrap, &slots, 1);
	
	// // 컬링 모드는 논컬링
	G_Device.SetRasterState(ERasterState::CullNone);
	// // 깊이 버퍼 읽기 쓰기 비활성화
	// G_Device.SetDepthStencilState(EDepthStencilState::DepthNone);

	float gameTime = GetWorld.GetGameTime();
	// // Time버퍼를(상수) 넘겨줘야한다.
	mShader->UpdateConstantData(InDeviceContext, CBuffer::NAME_CONSTANT_BUFFER_TIME, "GameTime", &gameTime);
}

void JMaterial_Wind::InitializeParams()
{
	if (!mMaterialParams.empty())
	{
		mMaterialParams.clear();

		mMaterialParams.push_back(FMaterialParam("BaseColorMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("NormalMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("AOMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("RoughnessMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MetallicMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("EmissiveMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MaskMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("OpacityMap",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));


		mMaterialParams.push_back(FMaterialParam("BaseColor",
												 EMaterialParamValue::Float3,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("NormalColor",
												 EMaterialParamValue::Float3,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("NormalIntensity",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Emission",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("GlobalRoughness",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MaskPower",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Metallic",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("MetallicConstant",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("NormalTiling",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Opacity",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Roughness",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("RoughnessMask",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Specular",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("TextureSize",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Tiling",
												 EMaterialParamValue::Float,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("TextureUsage",
												 EMaterialParamValue::Integer,
												 &FVector::ZeroVector,
												 true));

		Utils::Serialization::Serialize(NAME_MAT_WIND, this);
	}
}

#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"
#include "ShaderUtils.hlslinc"
// ------------------------------------------------------------------
// : Sky Sphere
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Settings
// Blend Mode : Opaque
// Shading Model : Unlit
// ------------------------------------------------------------------

// 텍스처 4장이 필요하다.
static const int g_TextureCount = 3;

struct SkyData
{
	// Sky VertexColor (블렌딩을 위한 색상)
	float4 SkyColor_01 /*= float4(0.15f, 0.15f, 0.15f, 1.f)*/;
	float4 SkyColor_02 /*= float4(0.02, 0.02, 0.02, 1.f)*/;

	// Cloud VertexColor (블렌딩을 위한 색상)
	float4 CloudColor_01 /*= float4(0.200000, 0.200000, 0.200000, 1.f)*/;
	float4 CloudColor_02 /*= float4(0.130136, 0.130136, 0.130136, 1.f)*/;

	float LightingStrength /*= 4.070328f*/;
};

struct InstanceData_SkySphere
{
	row_major matrix Transform : INST_TRANSFORM;
	row_major matrix InvTransform : INST_TRNASFORM_INVTRANS;

	SkyData SkyData : INST_SKY_DATA;
};

struct VertexIn_3D
{
	float3 Pos : POSITION;	// NDC(local) 좌표
	float2 UV : TEXCOORD0;	// TexCoord 좌표
	float4 Color : COLOR;
	float3 Normal : NORMAL;	// 법선 벡터
};

struct VertexOut_3D
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float3 Normal : NORMAL;

	SkyData SkyData : TEXCOORD1;
};

struct PixelInput_3D
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float3 Normal : NORMAL;

	SkyData SkyData : TEXCOORD1;
};

Texture2D g_BaseTextureArray[g_TextureCount] : register(t0);

SamplerState g_SamplerLinearWrap : register(s0);

VertexOut_3D VS(VertexIn_3D Input, InstanceData_SkySphere Instance)
{
	VertexOut_3D output;

	output.Pos = float4(Input.Pos, 1.f);
	output.Pos = mul(output.Pos, Instance.Transform);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// 깊이버퍼 w값 최댓값 설정

	output.UV     = Input.UV;
	output.Normal = Input.Normal;

	output.SkyData = Instance.SkyData;

	return output;
}

float3 LerpFloat3(float3 a, float3 b, float3 rgb)
{
	float3 result;
	result.r = lerp(a.r, b.r, rgb.r);
	result.g = lerp(a.g, b.g, rgb.g);
	result.b = lerp(a.b, b.b, rgb.b);
	return result;
}

float CustomSine(float input, float period, float phase)
{
	// period와 phase를 사용하여 언리얼의 Sine 노드와 유사한 동작을 구현
	return sin((input + phase) * (2 * 3.14159) / period);
}

float4 PS(PixelInput_3D Input) : SV_TARGET
{
	float2 cloud_01_UV = Panner(Input.UV * 2.f, float2(0.002f, 0), GameTime);
	float2 cloud_02_UV = Panner(Input.UV * 1.f, float2(0.005f, 0), GameTime);

	float3 sampleTexture = g_BaseTextureArray[0].Sample(g_SamplerLinearWrap, float2(0.002f, 0)).rgb;
	float3 cloud_01      = g_BaseTextureArray[1].Sample(g_SamplerLinearWrap, cloud_01_UV).rgb;
	float3 cloud_02      = g_BaseTextureArray[2].Sample(g_SamplerLinearWrap, cloud_02_UV).rgb;

	float3 lerpedCloudColor = Input.SkyData.CloudColor_01 * lerp(Input.SkyData.LightingStrength,
																 1.0f,
																 clamp(frac(CustomSine(GameTime, 10.f, 0.8f)), 0, 1));

	float3 alpha = WorldAlignedBlend(normalize(Input.Normal), -1.869581, 0.075672f);

	float3 emissiveColor = LerpFloat3(Input.SkyData.SkyColor_01.rgb, lerpedCloudColor.rgb, cloud_01);
	emissiveColor        = LerpFloat3(emissiveColor, lerpedCloudColor.rgb, sampleTexture);
	emissiveColor        = LerpFloat3(emissiveColor, Input.SkyData.CloudColor_02.rgb, cloud_02);
	emissiveColor        = LerpFloat3(emissiveColor,
							   Input.SkyData.SkyColor_02.rgb,
							   alpha);

	return float4(emissiveColor, 1.f);
}

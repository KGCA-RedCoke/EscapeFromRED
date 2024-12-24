#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"
#include "ShaderUtils.hlslinc"
// ------------------------------------------------------------------
// : Decal Color
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Settings
// Blend Mode : Alpha
// Shading Model : Lit
// ------------------------------------------------------------------

Texture2D    g_OpacityTexture : register(t0); // 텍스처 슬롯 0
SamplerState g_SamplerLinearWrap : register(s0); // 샘플러 슬롯 0

// 변하는 않는 변수는 그냥 상수
const float g_metallic = 0.f;
const float g_AO       = 1.f;

/// 준비물 (다 편집이 가능해야 함)
/// 1. Base Color (Color)
/// 2. Roughness (float)
/// 3. Opacty Texture & Value

PixelIn_Base VS(VertexIn_Base Input, InstanceData Instance)
{
	PixelIn_Base output;

	output.VertexColor = Input.VertexColor;
	float3 normal      = Input.Normal;

	output.WorldSpace = float4(Input.Pos, 1.f);
	output.WorldSpace = mul(output.WorldSpace, Instance.Transform);
	output.ClipSpace  = mul(output.WorldSpace, View);
	output.ClipSpace  = mul(output.ClipSpace, Projection);

	output.ViewDir = normalize(WorldCameraPos.xyz - output.WorldSpace.xyz);

	output.TexCoord = Input.TexCoord;
	output.Normal   = mul(normal, (float3x3)Instance.Transform);
	output.Tangent  = mul(Input.Tangent, (float3x3)Instance.Transform);
	output.Binormal = mul(Input.Binormal, (float3x3)Instance.Transform);

	return output;
}

float4 PS(PixelIn_Base Input) : SV_TARGET
{
	// float3 baseColor = Input.Material.BaseColor.rgb;
	// float  roughness = Input.Material.Roughness;
	float  opacity   = 1.f;
	// baseColor        = float3(0.354167f, 0.02538f, 0.f);

	opacity = g_OpacityTexture.Sample(g_SamplerLinearWrap, Input.TexCoord).r;
	opacity *= 1.f;
	// opacity = saturate(opacity);
	if (opacity < 0.1f) // 예: 0.1 미만은 그리지 않음
	{
		discard;
	}
	// 최종 색상: 베이스 컬러에 오파시티를 적용
	// float4 finalColor = float4(baseColor, opacity);

	return (1, 1, 1, 1);
}

#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"
#include "ShaderUtils.hlslinc"
// ------------------------------------------------------------------
// : Mirror (EFR게임 창문 전용)
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Settings
// Blend Mode : Translucent (DX11 : Alpha Blending)
// Shading Model : lit
// ------------------------------------------------------------------


// 창문 더럽히는 텍스처
Texture2D    g_DirtyTexture : register(t0);
SamplerState g_SamplerLinearWrap : register(s0);

const float metallic      = 3.448048f;
const float roughnessTile = 2.0f;
const float roughness     = 0.682485f;
const float opacity       = 0.6f;

PixelIn_Base VS(VertexIn_Base Input, InstanceData Instance)
{
	PixelIn_Base output;

	output.VertexColor = Input.VertexColor;
	output.WorldSpace  = float4(Input.Pos, 1.f);	// 로컬 좌표계

	output.WorldSpace = mul(output.WorldSpace, Instance.Transform);
	output.ViewDir    = normalize(WorldCameraPos.xyz - output.WorldSpace.xyz);
	output.ClipSpace  = mul(output.WorldSpace, View);
	output.ClipSpace  = mul(output.ClipSpace, Projection);
	output.TexCoord   = Input.TexCoord;
	output.Normal     = normalize(mul((float3x3)Instance.InvTransform, Input.Normal));
	output.Tangent    = normalize(mul((float3x3)Instance.InvTransform, Input.Tangent));
	output.Binormal   = normalize(cross(output.Normal, output.Tangent));

	return output;
}

float4 PS(PixelIn_Base Input) : SV_TARGET
{
	// 정규화된 벡터
	float3 normal   = normalize(Input.Normal);
	float3 lightDir = normalize(DirectionalLightPos - Input.WorldSpace.xyz);
	float3 viewDir  = normalize(Input.ViewDir);

	// **더러운 텍스처 샘플링**
	float3 dirtyFactor    = g_DirtyTexture.Sample(g_SamplerLinearWrap, Input.TexCoord * roughnessTile);
	float3 finalRoughness = roughness * dirtyFactor;

	// **PBR 반사율 계산**
	float  NdotL     = saturate(dot(normal, lightDir));       // 법선과 빛 방향의 내적
	float3 baseColor = float3(1.0, 1.0, 1.0);           // 기본 색상 (하얀 창문)
	float3 f0        = lerp(0.04f, 0.5f, metallic);
	float3 specular  = SpecularGGX(normal, viewDir, lightDir, finalRoughness, f0);
	float  ao        = 1.0f;                                    // 고정 Ambient Occlusion

	// **Fresnel 효과**
	float3 reflection = specular * pow(1.0 - saturate(dot(viewDir, normal)), finalRoughness * 5.0);

	// **최종 조명 계산**
	float3 diffuse    = baseColor * NdotL;                 // 확산광
	// float3 finalColor = ao * (diffuse + specular);    // AO 반영
	float3 finalColor = lerp(baseColor * diffuse, reflection, opacity);
	// **최종 색상 + 알파**
	return float4(dirtyFactor, opacity);  // 알파 채널에 opacity 적용
}

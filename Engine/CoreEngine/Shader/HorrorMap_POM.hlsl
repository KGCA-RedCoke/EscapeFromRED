#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"
#include "ShaderUtils.hlslinc"
// ------------------------------------------------------------------
// : HorrorMap POM(Parallax Occlusion Mapping) Shader
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Settings
// Blend Mode : Masked
// Shading Model : lit
// ------------------------------------------------------------------

cbuffer CMaterialBuffer : register(b9)
{
	float3 BaseColor : MAT_BASECOLOR;
	float  NormalColor_R : MAT_NORMAL_R;
	float  NormalColor_G : MAT_NORMAL_G;
	float  NormalColor_B : MAT_NORMAL_B;
	float  HeightRatio : MAT_HEIGHTRATIO;
	float  MinStep : MAT_MINSTEP;
	float  MaxStep : MAT_MAXSTEP;
	float  Roughness : MAT_ROUGHNESS1;
	float  GlobalRoughness : MAT_ROUGHNESS2;
	float  Specular : MAT_SPECULAR;
	float  Opacity : MAT_OPACITY;
};

Texture2D baseColorMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D aoMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D metallicMap : register(t4);
Texture2D emissiveMap : register(t5);
Texture2D opacityMap : register(t6);
Texture2D heightMap : register(t7);
Texture2D specularMap : register(t8);

SamplerState Sampler_Linear : register(s0);
SamplerState g_TextureSampler0 : register(s1);
SamplerState g_TextureSampler1 : register(s2);


PixelIn_Base VS(VertexIn_Base Input, InstanceData Instance)
{
	PixelIn_Base output;

	output.VertexColor = Input.VertexColor;
	output.WorldSpace  = mul(Input.Pos, Instance.Transform);
	output.ViewDir     = normalize(WorldCameraPos.xyz - output.WorldSpace.xyz);
	output.ClipSpace   = mul(output.WorldSpace, View);
	output.ClipSpace   = mul(output.ClipSpace, Projection);
	output.TexCoord    = Input.TexCoord;
	output.Normal      = normalize(mul((float3x3)Instance.InvTransform, Input.Normal));
	output.Tangent     = normalize(mul((float3x3)Instance.InvTransform, Input.Tangent));
	output.Binormal    = normalize(cross(output.Normal, output.Tangent));

	return output;
}


float4 PS(PixelIn_Base Input) : SV_TARGET
{

	float3 albedo       = BaseColor.rgb;
	float3 normal       = normalize(Input.Normal);
	float  ambientColor = 1.0f;
	float  roughness    = 0.5f;

	float3x3 tbn = float3x3(Input.Tangent, Input.Binormal, Input.Normal);

	const float roughnessFactor = Roughness;

	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	const float3 lightDir       = normalize(DirectionalLightPos.xyz);
	const float3 viewDir        = Input.ViewDir;
	const float3 viewDirTangent = mul(viewDir, tbn);

	float4 height = heightMap.Sample(Sampler_Linear, Input.TexCoord);

	const float2 texCoord = ParallaxOcclusionMapping(
													 Input.TexCoord,
													 viewDirTangent,
													 height,
													 HeightRatio,
													 MinStep,
													 MaxStep,
													 0
													);

	// Texture Map
	albedo *= baseColorMap.Sample(Sampler_Linear, texCoord);

	float4 normalColor = normalMap.Sample(Sampler_Linear, texCoord).rgba;
	if (normalColor.r * normalColor.g * normalColor.b < 0.9f)
	{
		normal = normalColor * 2.0f - 1.0f;
		normal = normalize(mul(normal, tbn));
	}
	normal *= float3(NormalColor_R, NormalColor_G, NormalColor_B);

	ambientColor = aoMap.Sample(Sampler_Linear, texCoord).r;
	roughness    = roughnessMap.Sample(Sampler_Linear, texCoord).g * roughnessFactor + GlobalRoughness;

	const float opacity = opacityMap.Sample(Sampler_Linear, texCoord).r * Opacity;

	float normDotLight = saturate(dot(normal, lightDir));

	float3 diffuse  = albedo * normDotLight;
	float3 f0       = lerp(0.04f, Specular, 0);
	float3 specular = SpecularGGX(normal, viewDir, lightDir, roughness, f0);

	// Point Lights and Spot Lights
	float3 finalPointLight = 0.0f;
	float3 finalSpotLight  = 0.0f;
	for (int i = 0; i < NumPointLights; ++i)
	{
		finalPointLight += ComputePointLight(Input.WorldSpace,
											 Input.Normal,
											 PointLight[i]);
	}

	for (int i = 0; i < NumSpotLights; ++i)
	{
		finalSpotLight += ComputeSpotLight(Input.WorldSpace,
										   Input.Normal,
										   SpotLight[i]);
	}

	diffuse += finalPointLight + finalSpotLight;

	// 주변광 (없으면 반사광이 없는곳은 아무것도 보이지 않음)
	float3 ambient = albedo * 0.1f * ambientColor; // ambientColor 반영

	// Final Color Calculation: Diffuse + Ambient + Specular
	float3 finalColor = diffuse + ambient + specular;

	return float4(finalColor, 1);

}

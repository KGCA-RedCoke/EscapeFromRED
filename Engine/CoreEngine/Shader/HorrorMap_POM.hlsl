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

struct FMaterialInstance_POM
{
	float3 BaseColor : MAT_BASECOLOR;
	float3 NormalColor : MAT_NORMAL;
	float  HeightRatio : MAT_HEIGHTRATIO;
	float  MinStep : MAT_MINSTEP;
	float  MaxStep : MAT_MAXSTEP;
	float  Roughness : MAT_ROUGHNESS1;
	float  GlobalRoughness : MAT_ROUGHNESS2;
	float  Specular : MAT_SPECULAR;
	float  Opacity : MAT_OPACITY;
};

struct InstanceData_POM
{
	row_major matrix      Transform : INST_TRANSFORM;
	row_major matrix      InvTransform : INST_TRNASFORM_INVTRANS;
	uint                  Flags : INST_FLAG;
	FMaterialInstance_POM Material : INST_MAT;
};

struct PixelIn_POM
{
	float4 ClipSpace : SV_POSITION;
	float4 WorldSpace : POSITION;
	float4 VertexColor : COLOR0;
	float2 TexCoord : TEXCOORD0;

	float3 ViewDir : TEXCOORD1;

	float3 Normal : TEXCOORD2;
	float3 Tangent : TEXCOORD3;
	float3 Binormal : TEXCOORD4;

	FMaterialInstance_POM Material : MATERIAL;
};

Texture2D baseColorMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D aoMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D metallicMap : register(t4);
Texture2D emissiveMap : register(t5);
Texture2D heightMap : register(t6);
Texture2D opacityMap : register(t7);
Texture2D specularMap : register(t8);

StructuredBuffer<float4> g_BoneTransforms : register(t10);

SamplerState Sampler_Linear : register(s0);
SamplerState g_TextureSampler0 : register(s1);
SamplerState g_TextureSampler1 : register(s2);


PixelIn_POM VS(VertexIn_Base Input, InstanceData_POM Instance)
{
	PixelIn_POM output;

	output.Material    = Instance.Material;
	output.VertexColor = Input.VertexColor;
	output.WorldSpace  = mul(Input.Pos, Instance.Transform);
	output.ViewDir     = normalize(WorldCameraPos.xyz - output.WorldSpace.xyz);
	output.ClipSpace   = mul(output.WorldSpace, View);
	output.ClipSpace   = mul(output.ClipSpace, Projection);
	output.TexCoord    = Input.TexCoord;
	output.Normal      = normalize(mul(Input.Normal, (float3x3)Instance.InvTransform));
	output.Tangent     = normalize(mul(Input.Tangent, (float3x3)Instance.InvTransform));
	output.Binormal    = normalize(cross(output.Normal, output.Tangent));

	return output;
}


float4 PS(PixelIn_POM Input) : SV_TARGET
{
	float3 albedo       = Input.Material.BaseColor.rgb;
	float3 normal       = normalize(Input.Normal);
	float  ambientColor = 1.0f;
	float  roughness    = 0.5f;

	float3x3 tbn = float3x3(Input.Tangent, Input.Binormal, Input.Normal);

	const float roughnessFactor = Input.Material.Roughness;

	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	const float3 lightDir       = normalize(DirectionalLightPos.xyz);
	const float3 viewDir        = normalize(Input.ViewDir);
	const float3 viewDirTangent = normalize(mul(viewDir, tbn));

	float4 height = heightMap.Sample(Sampler_Linear, Input.TexCoord);

	const float2 texCoord = Input.TexCoord;/*ParallaxOcclusionMapping(
																		Input.TexCoord,
																		viewDir,
																		height,
																		Input.Material.HeightRatio,
																		Input.Material.MinStep,
																		Input.Material.MaxStep,
																		0
																	   );*/

	// Texture Map
	albedo *= baseColorMap.Sample(Sampler_Linear, texCoord);

	float4 normalColor = normalMap.Sample(Sampler_Linear, texCoord).rgba;
	if (normalColor.r * normalColor.g * normalColor.b < 0.9f)
	{
		normal = normalColor * 2.0f - 1.0f;
		normal = normalize(mul(normal, tbn));
	}
	normal *= Input.Material.NormalColor;

	ambientColor = aoMap.Sample(Sampler_Linear, texCoord).r;
	roughness    = roughnessMap.Sample(Sampler_Linear, texCoord).g * roughnessFactor + Input.Material.GlobalRoughness;

	const float opacity = opacityMap.Sample(Sampler_Linear, texCoord).r * Input.Material.Opacity;

	float normDotLight = saturate(dot(normal, lightDir));

	float3 diffuse  = albedo * normDotLight;
	float3 f0       = lerp(0.04f, Input.Material.Specular, 0);
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

	return float4(finalColor, opacity);

}

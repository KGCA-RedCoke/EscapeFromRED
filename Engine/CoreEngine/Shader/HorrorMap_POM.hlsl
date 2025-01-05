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

Texture2D directionLightShadowMap : register(t10);
Texture2D pointLightShadowMap : register(t11);
SamplerState           Sampler_Linear : register(s0);
SamplerState           SamplerShadowMap: register (s1);
SamplerComparisonState SamplerComShadowMap: register (s2);


PixelIn_Base VS(VertexIn_Base Input, InstanceData Instance)
{
	PixelIn_Base output;

	output.VertexColor = Input.VertexColor;
	output.WorldSpace  = float4(Input.Pos, 1.f);	// 로컬 좌표계
	output.WorldSpace  = mul(output.WorldSpace, Instance.Transform);
	output.ViewDir     = normalize(WorldCameraPos.xyz - output.WorldSpace.xyz);
	output.ClipSpace   = mul(output.WorldSpace, View);
	output.ClipSpace   = mul(output.ClipSpace, Projection);
	output.TexCoord    = Input.TexCoord;
	output.Normal      = mul(float4(Input.Normal, 1), Instance.InvTransform);
	output.Tangent     = mul(float4(Input.Tangent.xyz, 1), Instance.InvTransform);
	output.Binormal    = cross(output.Normal, output.Tangent);

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
	const float3 lightDir       = DirectionalLightPos.xyz;
	const float3 viewDir        = Input.ViewDir;
	const float3 halfDir        = normalize(lightDir + viewDir);
	const float3 reflection     = -normalize(reflect(-viewDir, normal));
	const float3 viewDirTangent = mul(viewDir, tbn);

	float4 height = heightMap.Sample(Sampler_Linear, Input.TexCoord);

	const float2 texCoord = ParallaxOcclusionMapping(
													 Input.TexCoord,
													 viewDir,
													 height,
													 HeightRatio,
													 MinStep,
													 MaxStep,
													 0
													);

	// Texture Map
	albedo *= baseColorMap.Sample(Sampler_Linear, texCoord);

	float3 normalColor = 2 * normalMap.Sample(Sampler_Linear, texCoord) - 1;
	normal             = normalize(mul(normalColor * float3(NormalColor_R, NormalColor_G, NormalColor_B), tbn));

	ambientColor = aoMap.Sample(Sampler_Linear, texCoord).r;
	roughness    = roughnessMap.Sample(Sampler_Linear, texCoord).g * roughnessFactor + GlobalRoughness;

	const float opacity = opacityMap.Sample(Sampler_Linear, texCoord).r * Opacity;

	float  fLightAmount   = 1.0f;
	// float3 ShadowTexColor = Input.TexShadow.xyz / Input.TexShadow.w;
	//
	// // 그림자 맵 해상도에 따른 샘플링 간격 계산
	// const float fdelta = 1.0f / 4096;
	//
	// // 가중치 및 커널 크기 설정
	// float kernel[3][3] = {
	// 	{ 0.075f, 0.125f, 0.075f },
	// 	{ 0.125f, 0.200f, 0.125f },
	// 	{ 0.075f, 0.125f, 0.075f }
	// };
	// int iHalf = 1;
	//
	// // 3x3 PCF 필터링 수행
	// for (int v = -iHalf; v <= iHalf; v++)
	// {
	// 	for (int u = -iHalf; u <= iHalf; u++)
	// 	{
	// 		float2 vOffset = float2(u * fdelta, v * fdelta);
	// 		fLightAmount += kernel[v + iHalf][u + iHalf] * directionLightShadowMap.SampleCmpLevelZero(
	// 			SamplerComShadowMap,
	// 			ShadowTexColor.xy + vOffset,
	// 			ShadowTexColor.z
	// 		).r;
	// 	}
	// }
	// fLightAmount /= 3*3;	
	//
	// // 결과값 정규화
	// fLightAmount = saturate(fLightAmount);

	float normDotLight = saturate(dot(normal, lightDir)) * fLightAmount;

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

	// Final Color Calculation: Diffuse + Ambient + Specular
	float3 finalColor = diffuse + specular;
	finalColor        = lerp(finalColor, finalColor * ambientColor, 1);
	
	return float4(finalColor, 1);

}

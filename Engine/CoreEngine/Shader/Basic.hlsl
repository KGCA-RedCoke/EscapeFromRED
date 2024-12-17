#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"
#include "ShaderUtils.hlslinc"

Texture2D baseColorMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D aoMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D metallicMap : register(t4);
Texture2D emissiveMap : register(t5);
Texture2D opacityMap : register(t6);
Texture2D heightMap : register(t7);
Texture2D specularMap : register(t8);

StructuredBuffer<float4> g_BoneTransforms : register(t10);

SamplerState Sampler_Linear : register(s0);
SamplerState g_TextureSampler0 : register(s1);
SamplerState g_TextureSampler1 : register(s2);


PixelIn_Base VS(VertexIn_Base Input, InstanceData Instance)
{
	PixelIn_Base output;

	output.Material    = Instance.Material;
	output.VertexColor = Input.VertexColor;
	output.WorldSpace  = float4(Input.Pos, 1.f);	// 로컬 좌표계
	float4 localPos    = output.WorldSpace;
	float3 normal      = Input.Normal;

	if (Instance.Flags & FLAG_MESH_ANIMATED || Instance.Flags & FLAG_MESH_SKINNED)
	{
		for (int i = 0; i < 4; ++i)
		{
			const uint  boneIndex = (uint)Input.BoneIndices[i];	// 본 인덱스
			const float weight    = Input.BoneWeights[i] * 100.f;	// 가중치

			float4x4 boneTransform = FetchBoneTransform(boneIndex, g_BoneTransforms);	// 본의 변환 행렬을 가져온다.
			output.WorldSpace += (weight *
				mul(localPos, boneTransform));	// local(원래 메시 좌표) * boneTransform(애니메이션 변환 행렬) * weight(가중치)

			normal += mul(Input.Normal, (float3x3)boneTransform) *
					weight;	// normal(원래 메시 노말) * boneTransform(애니메이션 변환 행렬) * weight(가중치)
		}

		output.VertexColor = Input.BoneWeights;	// 가중치를 컬러로 표현
	}

	output.WorldSpace = mul(output.WorldSpace, Instance.Transform);
	output.ViewDir    = normalize(WorldCameraPos.xyz - output.WorldSpace.xyz);
	output.ClipSpace  = mul(output.WorldSpace, View);
	output.ClipSpace  = mul(output.ClipSpace, Projection);
	output.TexCoord   = Input.TexCoord;
	output.Normal     = mul(normal, (float3x3)Instance.InvTransform);
	output.Tangent    = mul(Input.Tangent, (float3x3)Instance.InvTransform);
	output.Binormal   = cross(output.Normal, output.Tangent);

	return output;
}


float4 PS(PixelIn_Base Input) : SV_TARGET
{
	float3 albedo       = Input.Material.BaseColor.rgb;
	float3 normal       = normalize(Input.Normal);
	float3 ambientColor = Input.Material.AO;
	float  roughness    = 1.f;
	float  metallic     = 0.04f;

	float3x3 tbn = float3x3(Input.Tangent, Input.Binormal, Input.Normal);

	const float roughnessFactor = Input.Material.Roughness;
	const float metallicFactor  = Input.Material.Metallic;
	const float emissiveFactor  = Input.Material.Emissive;

	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	const float3 lightDir = normalize(DirectionalLightPos.xyz);
	const float3 viewDir  = normalize(Input.ViewDir);
	const float2 texCoord = Input.TexCoord;

	albedo *= baseColorMap.Sample(Sampler_Linear, texCoord);

	float4 normalColor = normalMap.Sample(Sampler_Linear, texCoord).rgba;
	if (normalColor.r * normalColor.g * normalColor.b < 0.9f)
	{
		normal = normalColor * 2.0f - 1.0f;
		normal = normalize(mul(normal, tbn));
	}

	ambientColor = aoMap.Sample(Sampler_Linear, texCoord).r;
	roughness    = roughnessMap.Sample(Sampler_Linear, texCoord).g * roughnessFactor;
	metallic     = metallicMap.Sample(Sampler_Linear, texCoord).b * metallicFactor;

	const float3 emissive = emissiveMap.Sample(Sampler_Linear, texCoord).rgb * emissiveFactor;
	const float  opacity  = opacityMap.Sample(Sampler_Linear, texCoord).r * Input.Material.Opacity;

	float normDotLight = saturate(dot(normal, lightDir));

	float3 diffuse  = albedo * normDotLight;
	float3 f0       = lerp(0.04f, Input.Material.Specular, metallic);
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

	finalColor += emissive;

	return float4(finalColor, opacity);

}

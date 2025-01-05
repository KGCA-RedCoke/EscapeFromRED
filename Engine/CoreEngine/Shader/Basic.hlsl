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

Texture2D directionLightShadowMap : register(t10);
Texture2D pointLightShadowMap : register(t11);

SamplerState           Sampler_Linear : register(s0);
SamplerState           SamplerShadowMap: register (s1);
SamplerComparisonState SamplerComShadowMap: register (s2);


cbuffer CMaterialBuffer : register(b9)
{
	float3 BaseColor : MAT_BASECOLOR;
	float  AO : MAT_SPECULAR;
	float  Roughness : MAT_ROUGHNESS;
	float  Metallic : MAT_METALLIC;
	float  Emissive : MAT_EMISSIVE;
	float  Specular : MAT_SPECULAR;
	float  Opacity : MAT_OPACITY;
	uint   Flag : MAT_TEXTUREFLAG;
};

PixelIn_Base VS(VertexIn_Base Input, InstanceData Instance)
{
	PixelIn_Base output;

	output.VertexColor = Input.VertexColor;
	output.WorldSpace  = float4(Input.Pos, 1.f);	// 로컬 좌표계
	float4 localPos    = output.WorldSpace;
	float3 normal      = Input.Normal;
	float3 tangent     = Input.Tangent;

	uint animOffset       = Instance.SkeletalMesh.AnimData.x;
	uint nextAnimOffset   = Instance.SkeletalMesh.AnimData.y;
	uint animCurrentFrame = Instance.SkeletalMesh.AnimData.z;
	uint animNextFrame    = Instance.SkeletalMesh.AnimData.w;

	float boneCount   = Instance.SkeletalMesh.Data.x;
	float animDelta   = Instance.SkeletalMesh.Data.y;
	float blendWeight = Instance.SkeletalMesh.Data.z;

	if (boneCount > 0 || Instance.Flags & FLAG_MESH_SKINNED)
	{
		for (int i = 0; i < 4; ++i)
		{
			const uint  boneIndex = (uint)Input.BoneIndices[i];	// 본 인덱스
			const float weight    = Input.BoneWeights[i] * 100.f;	// 가중치

			float4x4 boneTransform = LerpAnimBoneMatrix(boneCount,
														animOffset,
														nextAnimOffset,
														animCurrentFrame,
														animNextFrame,
														animDelta,
														boneIndex,
														blendWeight);	// 본의 변환 행렬을 가져온다.
			output.WorldSpace += (weight *
				mul(localPos, boneTransform));	// local(원래 메시 좌표) * boneTransform(애니메이션 변환 행렬) * weight(가중치)

			normal += mul(Input.Normal, (float3x3)boneTransform) *
					weight;	// normal(원래 메시 노말) * boneTransform(애니메이션 변환 행렬) * weight(가중치)
			tangent += mul(Input.Tangent, (float3x3)boneTransform) *
					weight;	// tangent(원래 메시 탄젠트) * boneTransform(애니메이션 변환 행렬) * weight(가중치)
		}

		output.VertexColor = Input.BoneWeights;	// 가중치를 컬러로 표현
	}

	output.WorldSpace = mul(output.WorldSpace, Instance.Transform);
	output.ViewDir    = normalize(WorldCameraPos.xyz - output.WorldSpace.xyz);
	output.ClipSpace  = mul(output.WorldSpace, View);
	output.ClipSpace  = mul(output.ClipSpace, Projection);
	output.TexCoord   = Input.TexCoord;
	output.Normal     = mul(float4(normal, 1), Instance.InvTransform);
	output.Tangent    = mul(float4(Input.Tangent.xyz, 1), Instance.InvTransform);
	output.Binormal   = cross(output.Normal.xyz, output.Tangent.xyz);
	output.TexShadow  = mul(Input.Pos, Instance.ShadowTransform);

	return output;
}


float4 PS(PixelIn_Base Input) : SV_TARGET
{
	float3 albedo       = BaseColor.rgb;
	float3 normal       = normalize(Input.Normal);
	float3 ambientColor = AO;
	float  roughness    = 1.f;
	float  metallic     = 0.04f;

	float3x3 tbn = float3x3(Input.Tangent, Input.Binormal, Input.Normal);

	const float roughnessFactor = Roughness;
	const float metallicFactor  = Metallic;
	const float emissiveFactor  = Emissive;

	const float2 texCoord = Input.TexCoord;

	albedo *= baseColorMap.Sample(Sampler_Linear, texCoord);

	float3 normalColor = 2 * normalMap.Sample(Sampler_Linear, texCoord) - 1;
	normal             = normalize(mul(normalColor, tbn));

	ambientColor = aoMap.Sample(Sampler_Linear, texCoord).r;
	roughness    = roughnessMap.Sample(Sampler_Linear, texCoord).g * roughnessFactor;
	metallic     = metallicMap.Sample(Sampler_Linear, texCoord).b * metallicFactor;

	const float3 emissive = emissiveMap.Sample(Sampler_Linear, texCoord).rgb * emissiveFactor;
	const float  opacity  = opacityMap.Sample(Sampler_Linear, texCoord).r * Opacity;


	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	// float3 lightVec  = DirectionalLightPos.xyz - Input.WorldSpace.xyz;
	// float  lightDist = length(lightVec);
	// lightVec /= lightDist;
	// float3       halfway = normalize(Input.ViewDir + lightVec);
	// const float3 viewDir = Input.ViewDir;
	// float fLightAmount = 1.0f;
	// float NdotI        = max(0.0, dot(normal, lightVec));
	// float NdotH        = max(0.0, dot(normal, halfway));
	// float NdotO        = max(0.0, dot(normal, Input.ViewDir));
	//
	// const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0
	// float3       F0          = lerp(Fdielectric, albedo, metallic);
	// float3       F           = SchlickFresnel(F0, max(0.0, dot(halfway, viewDir)));
	// float3       kd          = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
	// float3       diffuseBRDF = kd * albedo;
	//
	// float  D            = NdfGGX(NdotH, roughness);
	// float3 G            = SchlickGGX(NdotI, NdotO, roughness);
	// float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NdotI * NdotO);
	//
	// float3 radiance = 1.0f;
	// /*radiance        = LightRadiance(DirectionalLightPos);*/
	//
	// float shadow = 1.0f;
	// float3 ShadowTexColor = Input.TexShadow.xyz / Input.TexShadow.w;
	// float depth = directionLightShadowMap.SampleCmpLevelZero(SamplerComShadowMap, ShadowTexColor.xy, ShadowTexColor.z).r;
	//
	// if (depth + 0.001 < ShadowTexColor.z)
	// {
	// 	shadow = .5f;
	// }
	//
	//
	// float3 diffuse = (diffuseBRDF + specularBRDF) * radiance * NdotI * shadow;

	const float3 lightDir   = DirectionalLightPos.xyz;
	const float3 viewDir    = Input.ViewDir;
	const float3 halfDir    = normalize(lightDir + viewDir);
	const float3 reflection = -normalize(reflect(-viewDir, normal));

	float normDotLight = saturate(dot(normal, lightDir));

	float3 diffuse  = albedo * normDotLight;
	float3 f0       = lerp(0.04f, Specular, metallic);
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

	finalColor = lerp(finalColor, finalColor * ambientColor, AO);

	finalColor += emissive;

	if (Flag & FLAG_USE_RIMLIGHT)
	{
		finalColor += ComputeRimLight(normal, viewDir, float3(0, 1, 0), 10.f, 10.f);
	}

	// finalColor += ComputeRimLight(normal, viewDir, float3(0, 1, 0), 10.f, 10.f);

	// return float4(fLightAmount, fLightAmount, fLightAmount, 1.0f);
	return float4(finalColor, opacity);

}

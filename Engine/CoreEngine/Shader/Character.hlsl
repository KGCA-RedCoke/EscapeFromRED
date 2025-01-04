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
Texture2D maskMap : register(t7);

SamplerState Sampler_Linear : register(s0);
SamplerState g_TextureSampler0 : register(s1);
SamplerState g_TextureSampler1 : register(s2);

cbuffer CMaterialBuffer : register(b9)
{
	float3 BaseColor : MAT_BASECOLOR;
	float  AO : MAT_SPECULAR;
	float  Roughness : MAT_ROUGHNESS;
	float  Metallic : MAT_METALLIC;
	float  Emissive : MAT_EMISSIVE;
	float  Specular : MAT_SPECULAR;
	float  Opacity : MAT_OPACITY;
	float  MaskColor1_R;
	float  MaskColor1_G;
	float  MaskColor1_B;
	float  MaskColor2_R;
	float  MaskColor2_G;
	float  MaskColor2_B;
	float  MaskColor3_R;
	float  MaskColor3_G;
	float  MaskColor3_B;
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

	return output;
}


float4 PS(PixelIn_Base Input) : SV_TARGET
{
	
	float3 albedo       = BaseColor.rgb;
	float3 normal       = normalize(Input.Normal);
	float3 ambientColor = AO;
	float  roughness    = 1.f;
	float  metallic     = 0.04f;
	float3 maskColor1 = {MaskColor1_R,MaskColor1_G,MaskColor1_B};
	float3 maskColor2 = {MaskColor2_R,MaskColor2_G,MaskColor2_B};
	float3 maskColor3 = {MaskColor3_R,MaskColor3_G,MaskColor3_B};

	
	float3x3 tbn = float3x3(Input.Tangent, Input.Binormal, Input.Normal);

	const float roughnessFactor = Roughness;
	const float metallicFactor  = Metallic;
	const float emissiveFactor  = Emissive; 

	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	const float3 lightDir   = DirectionalLightPos.xyz;
	const float3 viewDir    = Input.ViewDir;
	const float2 texCoord = Input.TexCoord;

	float3 rgbMask = maskMap.Sample(Sampler_Linear, texCoord);
	
	maskColor1 *= rgbMask.r;
	maskColor2 *= rgbMask.g;
	maskColor3 *= rgbMask.b;
	float3 maskColor = maskColor1 + maskColor2 + maskColor3;
	albedo += maskColor;
	
	albedo *= baseColorMap.Sample(Sampler_Linear, texCoord);

	float3 normalColor = 2 * normalMap.Sample(Sampler_Linear, texCoord) - 1;
	normal             = normalize(mul(normalColor, tbn));

	ambientColor = aoMap.Sample(Sampler_Linear, texCoord).r;
	roughness    = roughnessMap.Sample(Sampler_Linear, texCoord).g * roughnessFactor;
	metallic     = metallicMap.Sample(Sampler_Linear, texCoord).b * metallicFactor;

	const float3 emissive = emissiveMap.Sample(Sampler_Linear, texCoord).rgb * emissiveFactor;
	const float  opacity  = opacityMap.Sample(Sampler_Linear, texCoord).r * Opacity;

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

	// 주변광 (없으면 반사광이 없는곳은 아무것도 보이지 않음)
	float3 finalColor = diffuse + specular;

	finalColor = lerp(finalColor, finalColor * ambientColor, AO);

	finalColor += emissive;

	return float4(finalColor, opacity);

}

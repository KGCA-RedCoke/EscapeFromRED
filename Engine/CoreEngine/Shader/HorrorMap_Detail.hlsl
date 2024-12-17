#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"
#include "ShaderUtils.hlslinc"
// ------------------------------------------------------------------
// : HorrorMap Detail Shader
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Settings
// Blend Mode : Masked
// Shading Model : lit
// ------------------------------------------------------------------

struct FMaterialInstance_Detail
{
	float3 BaseColor : MAT_BASECOLOR;
	float3 NormalColor : MAT_NORMAL;
	float  NormalIntensity : MAT_NORMALINTENSITY;
	float  Emission : MAT_EMISSION;
	float  GlobalRoughness : MAT_ROUGHNESS2;
	float  MaskPower : MAT_MASKPOWER;
	float  Metallic : MAT_METALLIC;
	float  MetallicConstant : MAT_METALLICCONSTANT;
	float  NormalTiling : MAT_NORMALTILING;
	float  Opacity : MAT_OPACITY;
	float  Roughness : MAT_ROUGHNESS1;
	float  RoughnessMask : MAT_ROUGHNESSMASK;
	float  Specular : MAT_SPECULAR;
	float  TextureSize : MAT_TEXTURESIZE;
	float  Tiling : MAT_TILING;
};

struct InstanceData_Detail
{
	row_major matrix         Transform : INST_TRANSFORM;
	row_major matrix         InvTransform : INST_TRNASFORM_INVTRANS;
	uint                     Flags :INST_Flag;
	FMaterialInstance_Detail Material : INST_MAT;
};

struct PixelIn_Detail
{
	float4 ClipSpace : SV_POSITION;
	float4 WorldSpace : POSITION;
	float4 VertexColor : COLOR0;
	float2 TexCoord : TEXCOORD0;

	float3 ViewDir : TEXCOORD1;

	float3 Normal : TEXCOORD2;
	float3 Tangent : TEXCOORD3;
	float3 Binormal : TEXCOORD4;

	FMaterialInstance_Detail Material : MATERIAL;
};

Texture2D baseColorMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D aoMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D metallicMap : register(t4);
Texture2D emissiveMap : register(t5);
Texture2D maskMap : register(t6);
Texture2D opacityMap : register(t7);


StructuredBuffer<float4> g_BoneTransforms : register(t10);

SamplerState Sampler_Linear : register(s0);
SamplerState g_TextureSampler0 : register(s1);
SamplerState g_TextureSampler1 : register(s2);


PixelIn_Detail VS(VertexIn_Base Input, InstanceData_Detail Instance)
{
	PixelIn_Detail output;

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


float4 PS(PixelIn_Detail Input) : SV_TARGET
{
	float3 albedo       = Input.Material.BaseColor.rgb;
	float3 normal       = normalize(Input.Normal);
	float  ambientColor = 1.0f;
	float  roughness    = 0.5f;
	float  metallic     = Input.Material.Metallic;

	float3x3 tbn = float3x3(Input.Tangent, Input.Binormal, Input.Normal);

	const float roughnessFactor = Input.Material.Roughness;

	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	const float3 lightDir       = normalize(DirectionalLightPos.xyz);
	const float3 viewDir        = normalize(Input.ViewDir);
	const float3 viewDirTangent = mul(viewDir, tbn);

	const float2 texCoord = Input.TexCoord * Input.Material.Tiling;

	float4 detailMask = WorldAlignedTexture(maskMap,
											Sampler_Linear,
											Input.WorldSpace.xyz,
											Input.Normal,
											Input.Material.TextureSize);


	albedo *= baseColorMap.Sample(Sampler_Linear, texCoord);
	albedo *= pow(detailMask.rgb, Input.Material.MaskPower);

	float4 normalColor = normalMap.Sample(Sampler_Linear, texCoord).rgba;
	if (normalColor.r * normalColor.g * normalColor.b < 0.9f)
	{
		normal = normalColor * 2.0f - 1.0f;
		normal = normalize(mul(normal, tbn));
	}

	float3 detailNormal = WorldAlignedTexture(normalMap,
											  Sampler_Linear,
											  Input.WorldSpace.xyz,
											  Input.Normal,
											  Input.Material.NormalTiling).xyz;

	normal = BlendAngleCorrectNormal(normal, lerp(float3(0, 0, 1), detailNormal, Input.Material.NormalIntensity));
	normal *= Input.Material.NormalColor;

	ambientColor = aoMap.Sample(Sampler_Linear, texCoord).r;
	roughness    = roughnessMap.Sample(Sampler_Linear, texCoord).g * roughnessFactor;
	roughness    = roughness * (detailMask * Input.Material.RoughnessMask) + Input.Material.GlobalRoughness;
	metallic     = metallicMap.Sample(Sampler_Linear, texCoord).b + metallic;
	metallic     = metallic * Input.Material.MetallicConstant;

	const float opacity = opacityMap.Sample(Sampler_Linear, texCoord).r * Input.Material.Opacity;

	if (opacity < .2f || Input.Material.Opacity < .2f)
	{
		discard;
	}

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


	return float4(finalColor, opacity);

}

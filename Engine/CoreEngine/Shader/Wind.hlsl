#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"
#include "ShaderUtils.hlslinc"
// ------------------------------------------------------------------
// : HorrorMap Wind Shader
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Settings
// Blend Mode : Masked
// Shading Model : lit
// ------------------------------------------------------------------

// Wind parameters
float3 g_WindDirection = float3(10.0f, 0.0f, 0.0f); // 바람의 방향 (단위 벡터)
float g_WindStrength = 10.5f;   // 바람 강도
float g_WindFrequency = 0.2f;  // 바람 주파수
float g_WindSpeed = 1.5f;      // 바람 속도

cbuffer CMaterialBuffer : register(b9)
{
	float3 BaseColor : MAT_BASECOLOR;
	float  NormalColor_R : MAT_NORMAL_R;
	float  NormalColor_G : MAT_NORMAL_G;
	float  NormalColor_B : MAT_NORMAL_B;
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

Texture2D baseColorMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D aoMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D metallicMap : register(t4);
Texture2D emissiveMap : register(t5);
Texture2D maskMap : register(t6);
Texture2D opacityMap : register(t7);

SamplerState Sampler_Linear : register(s0);
SamplerState g_TextureSampler0 : register(s1);
SamplerState g_TextureSampler1 : register(s2);


PixelIn_Base VS(VertexIn_Base Input, InstanceData Instance)
{
	PixelIn_Base output;
	
	output.VertexColor = Input.VertexColor;
	output.WorldSpace  = float4(Input.Pos, 1.f);	// 로컬 좌표계
	float4 localPos    = output.WorldSpace;
	float3 normal      = Input.Normal;	
	
	// if (Instance.Flags & FLAG_MESH_ANIMATED || Instance.Flags & FLAG_MESH_SKINNED)
	// {
	// 	for (int i = 0; i < 4; ++i)
	// 	{
	// 		const uint  boneIndex = (uint)Input.BoneIndices[i];	// 본 인덱스
	// 		const float weight    = Input.BoneWeights[i] * 100.f;	// 가중치
	//
	// 		float4x4 boneTransform = FetchBoneTransform(boneIndex);	// 본의 변환 행렬을 가져온다.
	// 		output.WorldSpace += (weight *
	// 			mul(localPos, boneTransform));	// local(원래 메시 좌표) * boneTransform(애니메이션 변환 행렬) * weight(가중치)
	//
	// 		normal += mul(Input.Normal, (float3x3)boneTransform) *
	// 				weight;	// normal(원래 메시 노말) * boneTransform(애니메이션 변환 행렬) * weight(가중치)
	// 	}
	//
	// 	output.VertexColor = Input.BoneWeights;	// 가중치를 컬러로 표현
	// }
// 바람 효과 계산 (더 큰 진폭과 주파수로 역동적인 움직임 구현)
	float windEffect = sin(dot(Input.Pos.xz, g_WindDirection.xz) * (g_WindFrequency * 2.5f) + GameTime * (g_WindSpeed * 3.0f)) 
	                   * (g_WindStrength * 0.9f);

	// 위치 기반 추가 효과 (강도를 높이고 패턴을 복합적으로 조정)
	float positionalEffect = sin(Input.Pos.x * 1.2f + GameTime * 8.0f)
	                         * cos(Input.Pos.z * 0.8f - GameTime * 6.f)*2.5f;
	windEffect += positionalEffect * 0.5f; // 위치 기반 효과 강도 증가

	// Z축과 X축에도 바람 효과를 일부 적용해 더 자연스러운 움직임 구현
	output.WorldSpace.x += windEffect * 0.3f;
	output.WorldSpace.y += windEffect; // Y축 중심으로 큰 흔들림 효과
	output.WorldSpace.z += windEffect * 0.25f;

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
	float3 albedo       = BaseColor.rgb;
	float3 normal       = normalize(Input.Normal);
	float  ambientColor = 1.0f;
	float  roughness    = 0.5f;
	float  metallic     = Metallic;

	float3x3 tbn = float3x3(Input.Tangent, Input.Binormal, Input.Normal);

	const float roughnessFactor = Roughness;

	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	const float3 lightDir       = normalize(DirectionalLightPos.xyz);
	const float3 viewDir        = normalize(Input.ViewDir);
	const float3 viewDirTangent = mul(viewDir, tbn);

	const float2 texCoord = Input.TexCoord * Tiling;

	float4 detailMask = WorldAlignedTexture(maskMap,
											Sampler_Linear,
											Input.WorldSpace.xyz,
											Input.Normal,
											TextureSize);


	albedo *= baseColorMap.Sample(Sampler_Linear, texCoord);
	albedo *= pow(detailMask.rgb, MaskPower);

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
											  NormalTiling).xyz;

	normal = BlendAngleCorrectNormal(normal, lerp(float3(0, 0, 1), detailNormal, NormalIntensity));
	normal *= float3(NormalColor_R, NormalColor_G, NormalColor_B);

	ambientColor = aoMap.Sample(Sampler_Linear, texCoord).r;
	roughness    = roughnessMap.Sample(Sampler_Linear, texCoord).g * roughnessFactor;
	roughness    = roughness * (detailMask * RoughnessMask) + GlobalRoughness;
	metallic     = metallicMap.Sample(Sampler_Linear, texCoord).b + metallic;
	metallic     = metallic * MetallicConstant;

	float opacity = opacityMap.Sample(Sampler_Linear, texCoord).r * Opacity;

	if (opacity < 0.3f)
	{
		albedo *= opacity; // 밝기를 감소시키는 대신 텍스처 조정만 수행
	}

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
	float3 ambient = albedo * 0.1f * ambientColor; // ambientColor 반영

	// Final Color Calculation: Diffuse + Ambient + Specular
	float3 finalColor = diffuse + ambient + specular;


	return float4(finalColor, opacity);

}

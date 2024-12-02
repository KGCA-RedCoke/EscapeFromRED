#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"
#include "ShaderUtils.hlslinc"

Texture2D g_DiffuseTexture : register(t0);
Texture2D g_NormalTexture : register(t1);
Texture2D g_AmbientOcclusionTexture : register(t2);
Texture2D g_RoughnessTexture : register(t3);
Texture2D g_MetallicTexture : register(t4);

StructuredBuffer<float4> g_BoneTransforms : register(t5);

SamplerState g_LinearSampler : register(s0);
SamplerState g_TextureSampler0 : register(s1);
SamplerState g_TextureSampler1 : register(s2);


PixelIn_Base VS(VertexIn_Base Input, InstanceData Instance)
{
	PixelIn_Base output;

	output.Material    = Instance.Material;
	output.VertexColor = Input.VertexColor;
	output.WorldSpace   = float4(Input.Pos, 1.f);	// 로컬 좌표계
	float4 localPos    = output.WorldSpace;
	float3 normal      = Input.Normal;

	if (Instance.Material.Flag & FLAG_MESH_ANIMATED || Instance.Material.Flag & FLAG_MESH_SKINNED)
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

	output.WorldSpace = mul(output.WorldSpace, Instance.InstancePos);
	output.ViewDir    = normalize(WorldCameraPos.xyz - output.WorldSpace.xyz);
	output.ClipSpace  = mul(output.WorldSpace, View);
	output.ClipSpace  = mul(output.ClipSpace, Projection);
	output.TexCoord   = Input.TexCoord;
	output.Normal     = mul(normal, (float3x3)Instance.InstancePos);
	output.Tangent    = mul(Input.Tangent, (float3x3)Instance.InstancePos);
	output.Binormal   = mul(Input.Binormal, (float3x3)Instance.InstancePos);

	return output;
}


float4 PS(PixelIn_Base Input) : SV_TARGET
{
	// if (MeshFlags & FLAG_MESH_SKINNED)
	// {
	// 	// ! FOR DEBUG !
	// 	// 가중치 표현 (가중치는 잘 적용이 됐는데 스케일 100을 곱해줘야하는 이유?)
	// 	return float4(Input.VertexColor.rgb, 1);
	// }

	float3 diffuseColor = Input.Material.BaseColor.rgb;
	float3 normalColor  = normalize(Input.Normal /** 2.0f - 1.0f*/); // -1 ~ 1 사이로 정규화

	// 	R (Red): Ambient Occlusion (AO)
	//  G (Green): Roughness
	//  B (Blue): Metallic
	float ambientColor = Input.Material.Occlusion;
	float roughness    = Input.Material.Roughness;
	float metallic     = Input.Material.Metallic;

	// Texture Map
	if (Input.Material.Flag & TEXTURE_USE_DIFFUSE)
	{
		diffuseColor = g_DiffuseTexture.Sample(g_LinearSampler, Input.TexCoord);
	}
	if (Input.Material.Flag & TEXTURE_USE_NORMAL)
	{
		normalColor = g_NormalTexture.Sample(g_LinearSampler, Input.TexCoord).rgb;
		normalColor = normalize(normalColor * 2.0f - 1.0f);
	}
	if (Input.Material.Flag & TEXTURE_USE_AMBIENTOCCLUSION)
	{
		ambientColor = g_AmbientOcclusionTexture.Sample(g_LinearSampler, Input.TexCoord).r;
	}
	if (Input.Material.Flag & TEXTURE_USE_ROUGHNESS)
	{
		roughness = g_RoughnessTexture.Sample(g_LinearSampler, Input.TexCoord).g;
	}
	if (Input.Material.Flag & TEXTURE_USE_METALLIC)
	{
		metallic = g_MetallicTexture.Sample(g_LinearSampler, Input.TexCoord).b;
	}

	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	float3 lightDir = normalize(DirectionalLightPos.xyz);
	float  NdotL    = saturate(dot(normalColor, lightDir));
	float3 diffuse  = NdotL * diffuseColor.rgb * DirectionalLightColor.rgb;
	float3 specular = 0;

	// 난반사광이 없으면 애초에 반사되는 색상이 없음
	if (diffuse.x > 0)
	{
		// 노말과 라이트의 반사각
		float3 reflection = reflect(lightDir, normalColor);
		float3 viewDir    = normalize(Input.ViewDir);

		// Specular는 카메라뷰와 반사각의 내적값을 제곱(할수록 광 나는 부분이 줄어듦) 사용
		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 1 / roughness);
	}

	// 주변광 (없으면 반사광이 없는곳은 아무것도 보이지 않음)
	float3 ambient = 0.1 * diffuseColor;

	for (int i = 0; i < 1; ++i)
	{
		float3 pointLight = ComputePointLight(Input.WorldSpace,
											 Input.Normal,
											 PointLight[i]);
		diffuse += pointLight;
		specular += pointLight;
	}

	float3 finalColor = float3(diffuse + ambient + (specular * metallic));
	// finalColor.rgb    = lerp(finalColor.rgb, finalColor.rgb * metallic, metallic);

	// return float4(normalColor, 1.f);
	return float4(finalColor, 1.f);

}

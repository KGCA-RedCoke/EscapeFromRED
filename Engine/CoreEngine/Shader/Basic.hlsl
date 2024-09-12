#include "ShaderUtils.hlsl"

Texture2D g_AlbedoTexture : register(t0);
Texture2D g_NormalTexture : register(t1);
Texture2D g_AmbientOcclusionTexture : register(t2);
Texture2D g_MetallicTexture : register(t3);
Texture2D g_RoughnessTexture : register(t4);
Texture2D g_SpecularTexture : register(t5);

SamplerState g_DiffuseSampler : register(s0);
SamplerState g_TextureSampler0 : register(s1);
SamplerState g_TextureSampler1 : register(s2);

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer LightConstantBuffer : register(b1)
{
	float4 WorldCameraPos;
	float4 WorldLightPos;
	float4 LightColor;
}

struct VertexShaderInput
{
	float3 Pos : POSITION;	// NDC(local) 좌표
	float2 UV : TEXCOORD0;	// UV 좌표
	float4 Color : COLOR;	// 정점 컬러(대부분은 사용 안함)

	float3 Normal : NORMAL;	// 법선 벡터
	float3 Tangent : TANGENT;	// 접선 벡터
	float3 Binormal : BINORMAL;	// 이접 벡터
};

struct PixelShaderInput
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
	float2 UV : TEXCOORD0;
	float3 LightDir : TEXCOORD1;
	float3 ViewDir : TEXCOORD2;

	float3 Normal : TEXCOORD3;
	float3 Tangent : TEXCOORD4;
	float3 Binormal : TEXCOORD5;
};

PixelShaderInput VS(VertexShaderInput Input)
{
	PixelShaderInput output;

	output.Pos = float4(Input.Pos, 1.f);

	output.Pos = mul(output.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	float3 worldPos      = mul(Input.Pos, (float3x3)World);
	float3 lightDir      = worldPos.xyz - WorldLightPos.xyz;
	float3 viewDir       = worldPos.xyz - WorldCameraPos.xyz;
	float3 worldNormal   = mul(Input.Normal, (float3x3)World);
	float3 worldTangent  = mul(Input.Tangent, (float3x3)World);
	float3 worldBinormal = mul(Input.Binormal, (float3x3)World);

	output.Color    = Input.Color;
	output.UV       = Input.UV;
	output.LightDir = normalize(lightDir);
	output.ViewDir  = normalize(viewDir);
	output.Normal   = normalize(worldNormal);
	output.Tangent  = normalize(worldTangent);
	output.Binormal = normalize(worldBinormal);

	return output;
}


float4 PS(PixelShaderInput Input) : SV_TARGET
{
	// 	R (Red): Ambient Occlusion (AO)
	//  G (Green): Roughness
	//  B (Blue): Metallic
	// float  ambientColor  = ambientOcclusionTexture.Sample(g_DiffuseSampler, Input.UV).r;
	// float  metallic      = metallicTexture.Sample(g_DiffuseSampler, Input.UV).b;
	// float  roughness     = roughnessTexture.Sample(g_DiffuseSampler, Input.UV).g;
	// float  specularColor = specularTexture.Sample(g_DiffuseSampler, Input.UV).r;
	float4 albedoColor = g_AlbedoTexture.Sample(g_DiffuseSampler, Input.UV);
	float3 normalColor = g_NormalTexture.Sample(g_DiffuseSampler, Input.UV).rgb;
	normalColor        = normalize(normalColor * 2.0f - 1.0f); // -1 ~ 1 사이로 정규화

	float3x3 TBN       = float3x3(Input.Tangent, Input.Binormal, Input.Normal);
	TBN                = transpose(TBN);
	float3 worldNormal = mul(TBN, normalColor);

	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	float3 lightDir = normalize(Input.LightDir);
	float3 diffuse  = saturate(dot(-lightDir, worldNormal));
	diffuse         = LightColor.rgb * albedoColor.rgb * diffuse;
	float3 specular = 0;

	// 난반사광이 없으면 애초에 반사되는 색상이 없음
	if (diffuse.x > 0)
	{
		// 노말과 라이트의 반사각
		float3 reflection = reflect(lightDir, worldNormal);
		float3 viewDir    = normalize(Input.ViewDir);

		// Specular는 카메라뷰와 반사각의 내적값을 제곱(할수록 광 나는 부분이 줄어듦) 사용
		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 20.0f);
	}

	// 주변광 (없으면 반사광이 없는곳은 아무것도 보이지 않음)
	float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedoColor.rgb;
	//
	float3 finalColor = float3(diffuse + ambient + specular);
	// finalColor.rgb    = lerp(finalColor.rgb, finalColor.rgb * metallic, metallic);

	return float4(finalColor, 1.0f);
}

float4 PS_COLOR(PixelShaderInput Input) : SV_TARGET
{
	return Input.Color;
}

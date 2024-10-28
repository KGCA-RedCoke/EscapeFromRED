#include "CommonConstantBuffers.hlslinc"
#include "CommonTextures.hlslinc"

struct VertexIn
{
	float3 Pos : POSITION;	// NDC(local) 좌표
	float2 UV : TEXCOORD0;	// UV 좌표
	float4 Color : COLOR;	// 정점 컬러(대부분은 사용 안함)

	float3 Normal : NORMAL;	// 법선 벡터
	float3 Tangent : TANGENT;	// 접선 벡터
	float3 Binormal : BINORMAL;	// 이접 벡터
};

struct VertexOut
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

VertexOut VS(VertexIn Input)
{
	VertexOut output;

	output.Pos = float4(Input.Pos, 1.f);
	output.Pos = mul(output.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	float3 worldPos      = mul(Input.Pos, (float3x3)World);
	float3 lightDir      = worldPos.xyz - DirectionalLightPos.xyz;
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

/**
 * 기본 렌더링 (텍스처 사용)
 */
float4 PS(VertexOut Input) : SV_TARGET
{
	float4 finalColor;

	// 지형 색상 맵
	float3 diffuseMap = g_DiffuseTexture.Sample(g_DiffuseSampler, Input.UV).rgb;

	// 지형 노말 맵
	float3 normalMap = g_NormalTexture.Sample(g_NormalSampler, Input.UV).rgb;

	// Directional Light 방향
	float3 lightDirection = normalize(DirectionalLightPos).xyz;

	// 빛과 법선 벡터 사이의 각도 (이걸로 빛의 세기를 계산) 
	float normalDotLight = saturate(dot(normalize(normalMap.rgb), -lightDirection));

	// 빛(Directional)의 영향을 받는 Albedo 색상
	float diffuseColor = diffuseMap * normalDotLight * DirectionalLightColor;

	float3 ambientColor = 0.1f;

	// 난반사광이 없으면 Specular 계산을 하지 않는다.
	if (diffuseColor.r > 0)
	{
		// 반사광 벡터
		float3 reflect = reflect(lightDirection, normalMap.rgb);

		// 뷰 방향
		float3 viewDirection = normalize(Input.ViewDir);

		// 반사광과 뷰 각도 TODO: (세기는 파라미터로 받아야 함)
		float specular = pow(saturate(dot(reflect, viewDirection)), 32);

		// 반사광 색상
		float3 specularColor = DirectionalLightColor * specular;

		// 최종 색상
		finalColor = float4(diffuseColor + specularColor + ambientColor, 1);
	}
	else
	{
		finalColor = float4(diffuseColor + ambientColor, 1);
	}


	return finalColor;
}


/**
 * 노말 표현
 */
float4 PS_Normal(VertexOut Input) : SV_TARGET
{
	float3 normalColor = normalize(Input.Normal * 2.0f - 1.0f); // -1 ~ 1 사이로 정규화
	return float4(normalColor, 1);
}

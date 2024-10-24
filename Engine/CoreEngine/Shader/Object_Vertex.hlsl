#include "CommonConstantBuffers.hlslinc"

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

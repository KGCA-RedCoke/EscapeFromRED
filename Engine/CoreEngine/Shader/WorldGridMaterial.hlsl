// #include "ShaderUtils.hlsl"

Texture2D g_DefaultGridTexture : register(t0);
Texture2D g_NormalTexture : register(t1);

Texture2D g_DepthTexture : register(t2);

SamplerState g_TextureSampler : register(s0);

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
};

struct VertexShaderInput
{
	float3 Pos : POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR;

	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct PixelShaderInput
{
	float4 Pos : SV_POSITION;
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

	output.UV       = Input.UV;
	output.LightDir = normalize(lightDir);
	output.ViewDir  = normalize(viewDir);
	output.Normal   = normalize(worldNormal);
	output.Tangent  = normalize(worldTangent);
	output.Binormal = normalize(worldBinormal);

	return output;
}

float CameraDepthFade(float FadeLength, float FadeOffset)
{
	float output = 0.0f;

	// float pixelDepth = g_DepthTexture.Sample(g_TextureSampler, uv).r;

	// output = pixelDepth - FadeOffset;
	// output = output / FadeLength;
	// output = clamp(output, 0.0f, 1.0f);

	return 0.5f;
}

float4 PS(PixelShaderInput Input) : SV_Target
{
	float cameraDepthFade = CameraDepthFade(1500.0f, 24.0f);

	float2 uv = Input.UV / 2;

	float defaultGrid = g_DefaultGridTexture.Sample(g_TextureSampler, uv).g;

	float3 normal = g_NormalTexture.Sample(g_TextureSampler, uv * 20).rgb;
	normal        = normal * float3(0.3f, 0.3f, 1.0f);

	float3x3 TBN       = float3x3(Input.Tangent, Input.Binormal, Input.Normal);
	TBN                = transpose(TBN);
	float3 worldNormal = mul(TBN, normal);

	float4 checkerBoardSubDivision = g_DefaultGridTexture.Sample(g_TextureSampler, uv * 10.0f).r;
	float  checkerBoardLerp1       = lerp(checkerBoardSubDivision.g, 1.0f, cameraDepthFade);
	float  checkerBoardLerp2       = lerp(checkerBoardSubDivision.g, 0.f, cameraDepthFade);

	float noise = g_DefaultGridTexture.Sample(g_TextureSampler, Input.UV).r;
	noise       = lerp(0.4f, 1.0f, noise);
	noise       = lerp(noise, 1 - noise, checkerBoardLerp1);

	float4 baseColor = noise * lerp(0.295f, 0.66f, lerp(float4(defaultGrid + checkerBoardLerp2, 0, 0, 1), 0.5f, 0.5f)) * 0.5f;

	float3 lightDir = normalize(Input.LightDir);
	float3 diffuse  = saturate(dot(-lightDir, worldNormal));
	diffuse         = /*LightColor.xyz*/float3(1.f, 0.976f, 0.992f) * baseColor.rgb * diffuse;
	float3 specular = 0;

	if (diffuse.x > 0)
	{
		float3 reflection = reflect(lightDir, worldNormal);
		float3 viewDir    = normalize(Input.ViewDir);

		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 20.0f);
	}

	float3 ambient = float3(0.1f, 0.1f, 0.1f) * baseColor.rgb;
	//
	float3 finalColor = float3(diffuse + ambient + specular);
	// finalColor.rgb    = lerp(finalColor.rgb, finalColor.rgb * metallic, metallic);

	return float4(finalColor, 1.0f);

	// return float4(baseColor, 1.0f);
}

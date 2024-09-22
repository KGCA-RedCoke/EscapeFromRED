cbuffer ScreenSizeConstantBuffer : register(b0)
{
	float4 screenSize;
};

Texture2D screenTexture : register(t0);
SamplerState samplerState : register(s0);

struct VertexShaderInput
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VertexShaderOutput VS(VertexShaderInput input)
{
	VertexShaderOutput output;
	output.position = float4(input.position, 1.0);
	output.uv = input.uv;
	return output;
}

float4 PS(VertexShaderOutput input) : SV_Target
{
	// 텍스처 샘플링과 Luma 계산
	float3 rgbNW = screenTexture.Sample(samplerState, input.uv + (float2(-1.0, -1.0) / screenSize.xy)).xyz;
	float3 rgbNE = screenTexture.Sample(samplerState, input.uv + (float2(1.0, -1.0) / screenSize.xy)).xyz;
	float3 rgbSW = screenTexture.Sample(samplerState, input.uv + (float2(-1.0, 1.0) / screenSize.xy)).xyz;
	float3 rgbSE = screenTexture.Sample(samplerState, input.uv + (float2(1.0, 1.0) / screenSize.xy)).xyz;
	float3 rgbM = screenTexture.Sample(samplerState, input.uv).xyz;

	// 휘도(Luminance) 계산
	float3 luma = float3(0.299, 0.587, 0.114); // 인간의 눈에 기반한 가중치
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM = dot(rgbM, luma);

	// Luma 최소값과 최대값 계산
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	// 엣지 방향 탐지
	float2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE)); // 수평 방향
	dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));  // 수직 방향

	// 엣지 강도 조정
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * 2.0), 0.001);
	float dirMin = min(1.0, (lumaMin / lumaMax) * dirReduce);
	dir = normalize(dir) * dirMin; // 방향 벡터 정규화 및 조정

	// 오프셋 계산
	float2 offset = dir * (1.0 / screenSize);

	// 주위 픽셀들의 색상 보간
	float3 rgbA = (1.0 / 2.0) * (screenTexture.Sample(samplerState, input.uv + offset).xyz + screenTexture.Sample(samplerState, input.uv - offset).xyz);
	float3 rgbB = rgbA * 0.5 + (1.0 / 4.0) * (screenTexture.Sample(samplerState, input.uv + offset * 2.0).xyz + screenTexture.Sample(samplerState, input.uv - offset * 2.0).xyz);

	// 최종 색상 계산
	float3 finalColor = lerp(rgbM, rgbB, 0.75); // 원래 색상과 보간된 색상을 혼합

	return float4(finalColor, 1.0); // 최종 색상 반환
}
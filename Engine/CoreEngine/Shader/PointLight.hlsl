// Constant Buffer - 점광원의 위치 및 색상 정보
cbuffer LightData : register(b0)
{
	float3 lightPosition;    // 광원의 세계 좌표 (월드 스페이스)
	float  lightIntensity;   // 광원의 강도
	float4 lightColor;       // 광원의 색상 (RGBA)
	float4 cameraPosition;   // 카메라 위치 (뷰 공간)
};

// Vertex Shader
struct VSInput
{
	float3 position : POSITION; // 임의의 입력 위치
};

struct PSInput
{
	float4 position : SV_POSITION; // 클립 공간 위치
	float2 uv       : TEXCOORD0;   // UV 좌표
};

PSInput VSMain(VSInput input)
{
	PSInput output;
	output.position = float4(input.position, 1.0); // 전달만 수행
	output.uv = input.position.xy * 0.5 + 0.5;    // UV 매핑
	return output;
}

// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
	// 화면 공간 UV 좌표를 기준으로 광원 위치와의 거리 계산
	float2 screenUV = input.uv;               // 0 ~ 1 범위의 UV 좌표
	float2 lightScreenUV = lightPosition.xy;  // 빛의 화면 공간 좌표 (사전 계산된 값)

	float distance = length(screenUV - lightScreenUV); // 화면에서 광원 중심까지의 거리

	// 빛의 감쇠 (Fresnel + 거리 기반 감쇠)
	float attenuation = saturate(1.0 - distance * 2.0); // 가까울수록 밝음
	attenuation = pow(attenuation, 3.0);               // 부드러운 감쇠 효과

	// 빛나는 효과 적용 (광원의 색상 및 강도 사용)
	float3 glow = lightColor.rgb * lightIntensity * attenuation;

	return float4(glow, attenuation); // Alpha는 감쇠 값
}


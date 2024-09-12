// Base Diffuse Light(난반사광) Shader
// lambert 모델
// 표면의 법선과 입사광이 이루각의 cos값 -> 난반사광의 값 (대략)
// 따라서 표면과 빛이 이루각이 90도 이상이거나 -90도 이하일 때 0으로 처리
// cos은 비싸기 때문에 dot으로 대체 (반사광을 구하 때 벡터의 길이는 중요X)

// 입사광의 벡터, 표면법선의 내적은 Vertex, Pixel 어느 셰이더에서?
// -----> 상관 없다. (계산은 어디서나 가능)

// Vetex 단
// 정점마다 두 벡터를 Dot, 결과를 PixelInput으로 반환
// Rasterizer/보간기에서 값을 보간하여 Pixel Shader에 전달

// Pixel 단
// 표면법선의 정보를 Pixel Shader 입력으로 받는다
// 보간된 법선을 입사광과 Dot

// Vertex단에서는 DotProduct를 총 3번실행한다(삼각형 기준).
// 하지만 Pixel에서는? 
// Pixel에서는 삼각형이 화면의 픽셀을 차지하는 만큼 실행

// -----> 성능이 중요? (vertex shader) high-poly라면 괜찮
// -----> 품질이 중요? (pixel shader) 

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix Model : packoffset(c0);
	matrix View : packoffset(c4);
	matrix Projection : packoffset(c8);
};

cbuffer LightConstantBuffer : register(b1)
{
	float4 WorldLightPos;
}

struct VertexShaderInput
{
	float4 Position : POSITION;
	float3 Normal :NORMAL;
};

struct PixelShaderInput
{
	float4 Position : POSITION;
	float3 Diffuse : TEXCOORD1;
};

PixelShaderInput VS(VertexShaderInput Input)
{
	PixelShaderInput Output;

	Output.Position = mul(Input.Position, Model);

	// 빛의 입사각 벡터 계산 (local -> world로 변환된 object위치에서 계산)
	float3 lightDir = Output.Position.xyz - WorldLightPos.xyz;
	// dot product를 위한 정규화
	lightDir = normalize(lightDir);

	Output.Position = mul(Output.Position, View);
	Output.Position = mul(Output.Position, Projection);

	float3 worldNormal = mul(Input.Normal, (float3x3)Model);
	worldNormal        = normalize(worldNormal);

	// 입사광의 방향이 표면쪽을 향하면 값이 -가 됨
	Output.Diffuse = dot(-lightDir, worldNormal);

	return Output;
}

float4 PS(PixelShaderInput Input) : COLOR
{
	// saturate는 0 - 1사이의 값으로 값을 clamp (비용이 쌈)
	float3 diffuse = saturate(Input.Diffuse);
	return float4(diffuse, 1);
}

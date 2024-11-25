// ------------------------------------------------------------------
// : 기본 버텍스 컬러로 렌더링하는 쉐이더
// ------------------------------------------------------------------

cbuffer SimpleConstantBuffer : register(b0)
{
	row_major matrix World;
	row_major matrix View;
	row_major matrix Projection;
};

struct VS_INPUT
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
};

struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
};

VS_OUTPUT VS(VS_INPUT Input)
{
	VS_OUTPUT output;

	output.p = float4(Input.p, 1.f);
	output.p = mul(output.p, World);
	output.p = mul(output.p, View);
	output.p = mul(output.p, Projection);

	output.n = Input.n;
	output.c = Input.c;

	return output;
}


float4 PS(VS_OUTPUT Input) : SV_TARGET
{
	return Input.c;
}

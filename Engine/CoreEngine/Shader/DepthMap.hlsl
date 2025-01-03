#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"

struct PCT_VS_OUTPUT_SHADOW
{
	float4 p : SV_POSITION;
	float2 d : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PCT_VS_OUTPUT_SHADOW VS(VertexIn_Base Input, InstanceData Instance)
{
	PCT_VS_OUTPUT_SHADOW output;
	output.p = mul(float4(Input.Pos, 1.0f), Instance.Transform);
	output.p = mul(output.p, View);
	output.p = mul(output.p, Projection);
	output.d = output.p.zw;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PCT_VS_OUTPUT_SHADOW input) : SV_Target
{
	// float shadow = clamp(input.d.x, 0.5, 1);
	// return float4(shadow, shadow, shadow, 1.0f);

	return float4(0, 0, 1, 1);
}

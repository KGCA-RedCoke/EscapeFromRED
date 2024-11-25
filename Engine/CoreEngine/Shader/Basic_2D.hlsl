#include "InputLayout.hlslinc"

Texture2D    g_BaseTexture : register(t0);
SamplerState g_TextureSampler0 : register(s0);

cbuffer Ortho2D : register(b0)
{
	matrix Projection;
};

PixelInput_2D VS(VertexIn_2D Input, uint InstanceID : SV_InstanceID)
{
	PixelInput_2D output;

	output.Pos = float4(Input.Pos, 1.f);
	output.Pos = mul(output.Pos, Projection);

	output.UV    = Input.UV;
	output.Color = Input.Color;

	return output;
}


float4 PS(PixelInput_2D Input) : SV_TARGET
{
	return Input.Color;
}

#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"

Texture2D    g_BaseTexture : register(t0);
SamplerState g_TextureSampler0 : register(s0);

PixelInput_2D VS(VertexIn_2D Input, uint InstanceID : SV_InstanceID)
{
	PixelInput_2D output;

	output.Pos = float4(Input.Pos, 1.f);
	output.Pos = mul(output.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);


	output.UV    = Input.UV;
	output.Color = Input.Color;

	return output;
}


float4 PS(PixelInput_2D Input) : SV_TARGET
{
	if (TEXTURE_USE_DIFFUSE & TextureUsageFlag)
	{
		return g_BaseTexture.Sample(g_TextureSampler0, Input.UV) * Input.Color;
	}
	else
	{
		return Input.Color;
	}
}

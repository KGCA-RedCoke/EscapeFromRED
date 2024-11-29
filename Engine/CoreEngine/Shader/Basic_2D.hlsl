#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"

Texture2D    g_BaseTexture : register(t0);
SamplerState g_TextureSampler0 : register(s0);

struct InstanceData_2D
{
	row_major matrix InstancePos : INST_TRANSFORM;
};

PixelInput_2D VS(VertexIn_2D Input, InstanceData_2D Instance)
{
	PixelInput_2D output;

	output.Pos = float4(Input.Pos, 1.f);
	output.Pos = mul(output.Pos, Instance.InstancePos);
	output.Pos = mul(output.Pos, Orthographic);

	output.UV    = Input.UV;
	output.Color = Input.Color;

	return output;
}


float4 PS(PixelInput_2D Input) : SV_TARGET
{
	return Input.Color;
}

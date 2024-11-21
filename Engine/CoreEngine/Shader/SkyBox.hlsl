// #include "CommonConstantBuffers.hlslinc"
// #include "InputLayout.hlslinc"
// // ------------------------------------------------------------------
// // : SkyBox
// // ------------------------------------------------------------------
//
// // 텍스처 6장이 필요하다.
// static const int g_TextureCount = 6;
//
//
// Texture2D g_BaseTexture : register(t0);
// Texture2D g_BaseTextureArray[g_TextureCount] : register(t1);
//
// SamplerState g_SamplerPointClamp : register(s0);
// SamplerState g_SamplerLinearWrap : register(s1);
//
// VertexOut_3D VS(VertexIn_3D Input, uint InstanceID : SV_InstanceID)
// {
// 	VertexOut_3D output;
//
// 	output.Pos = float4(Input.Pos, 1.f);
// 	output.Pos = mul(output.Pos, World);
// 	output.Pos = mul(output.Pos, View);
// 	output.Pos = mul(output.Pos, Projection);
//
//
// 	output.UV     = Input.UV;
// 	output.Normal = Input.Normal;
//
// 	return output;
// }
//
//
// float4 PS(PixelInput_3D Input) : SV_TARGET
// {
// 	if (TEXTURE_USE_DIFFUSE & TextureUsageFlag)
// 	{
// 		return g_BaseTexture.Sample(g_TextureSampler0, Input.UV) * Input.Color;
// 	}
// 	else
// 	{
// 		return Input.Color;
// 	}
// }

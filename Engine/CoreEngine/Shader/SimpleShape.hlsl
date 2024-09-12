// Texture2D    shaderTexture : register(t0);
// SamplerState SampleType : register(s0);
//
// cbuffer ModelViewProjectionConstantBuffer : register(b0)
// {
// 	matrix Model;
// 	matrix View;
// 	matrix Projection;
// };
//
// struct VertexShaderInput
// {
// 	float3 Pos : POSITION;
// 	float3 Normal : NORMAL;
// 	float4 Color : COLOR;
// 	float2 Tex : TEXCOORD0;
// };
//
// struct PixelShaderInput
// {
// 	float4 Pos : SV_Position;
// 	float3 Normal : NORMAL;
// 	float4 Color : COLOR0;
// 	float2 Tex : TEXCOORD0;
// };
//
// PixelShaderInput VS(VertexShaderInput Input)
// {
// 	PixelShaderInput output;
// 	output.Pos = float4(Input.Pos, 1.f);
//
// 	output.Pos = mul(output.Pos, Model);
// 	output.Pos = mul(output.Pos, View);
// 	output.Pos = mul(output.Pos, Projection);
//
// 	output.Normal = Input.Normal;
// 	output.Color  = Input.Color;
// 	output.Tex    = Input.Tex;
//
// 	return output;
// }
//
//
// float4 PS(PixelShaderInput Input) : SV_TARGET
// {
// 	return shaderTexture.Sample(SampleType, Input.Tex);
// }

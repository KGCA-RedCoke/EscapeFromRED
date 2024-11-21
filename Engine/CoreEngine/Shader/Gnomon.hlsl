// GnomonMaterial.hlsl
#include "CommonConstantBuffers.hlslinc"

cbuffer GnomonParams : register(b5)
{
    float4 GridColor;
    float4 GirdColor2;
    float4 CenterColor;
    float Scale;
    float LineThickness;
}


// Input structure for vertex shader
struct VSInput
{
	float3 Position : POSITION;
};

// Output structure for pixel shader
struct PSInput
{
	float4 Position : SV_POSITION;
	float3 WorldPos : TEXCOORD0;
};

// Vertex Shader
PSInput VS(VSInput input)
{
	PSInput output;

	output.Position = float4(input.Position, 1.f);

	// output.Position = mul(output.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
    
	output.WorldPos = input.Position.xyz; // Pass world position to pixel shader

	return output;
}

// Pixel Shader
float4 PS(PSInput input) : SV_Target
{
	// Normalize the world position to determine the direction
	float3 worldPos = normalize(input.WorldPos);

	// Determine color based on axis
	float4 color;
	if (abs(worldPos.x) > abs(worldPos.y) && abs(worldPos.x) > abs(worldPos.z)) {
		color = float4(1.0, 0.0, 0.0, 1.0); // Red for X-axis
	} 
	else if (abs(worldPos.y) > abs(worldPos.x) && abs(worldPos.y) > abs(worldPos.z)) {
		color = float4(0.0, 1.0, 0.0, 1.0); // Green for Y-axis
	} 
	else {
		color = float4(0.0, 0.0, 1.0, 1.0); // Blue for Z-axis
	}

	return color;
}

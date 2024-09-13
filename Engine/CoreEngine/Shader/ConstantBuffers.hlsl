cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer LightConstantBuffer : register(b1)
{
	float4 WorldCameraPos;
	float4 WorldLightPos;
	float4 LightColor;
}

cbuffer WolrdTimeConstantBuffer : register(b2)
{
	float4 WorldTime;
}
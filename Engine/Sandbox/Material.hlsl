cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer LightConstantBuffer : register(b1)
{
	float4 WorldLightPos;
	float4 LightColor;
}

cbuffer CameraConstantBuffer : register(b2)
{
	float4 WorldCameraPos;
}

cbuffer WolrdTimeConstantBuffer : register(b3)
{
	float4 WorldTime;
}
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix Model : packoffset(c0);
	matrix View : packoffset(c4);
	matrix Projection : packoffset(c8);
};

cbuffer LightConstantBuffer : register(b1)
{
	float4 WorldLightPos;
};

cbuffer CameraConstantBuffer : register(b2)
{
	float4 WorldCameraPos;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

struct PixelShaderInput
{
	float4 Position : POSITION;
	float3 Diffuse : TEXCOORD1;
	float3 ViewDirection : TEXCOORD2;
	float3 Reflection : TEXCOORD3;
};

PixelShaderInput VS(VertexShaderInput Input)
{
	PixelShaderInput Output;

	Output.Position = mul(Input.Position, Model);

	float3 lightDir = Output.Position.xyz - WorldLightPos.xyz;
	lightDir        = normalize(lightDir);

	float3 viewDir       = normalize(Output.Position.xyz - WorldCameraPos.xyz);
	Output.ViewDirection = viewDir;

	Output.Position = mul(Output.Position, View);
	Output.Position = mul(Output.Position, Projection);

	float3 worldNormal = mul(Input.Normal, (float3x3)Model);
	worldNormal        = normalize(worldNormal);

	Output.Diffuse    = dot(-lightDir, worldNormal);
	Output.Reflection = reflect(lightDir, worldNormal);

	return Output;
}

float4 PS(PixelShaderInput Input) : COLOR
{
	float3 diffuse = saturate(Input.Diffuse);

	float3 reflection = normalize(Input.Reflection);
	float3 viewDir    = normalize(Input.ViewDirection);
	float3 specular   = 0;

	if (diffuse.x > 0)
	{
		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 20.f);
	}
	float3 ambient = float3(0.1f, 0.1f, 0.1f);

	return float4(ambient + diffuse + specular, 1);
}

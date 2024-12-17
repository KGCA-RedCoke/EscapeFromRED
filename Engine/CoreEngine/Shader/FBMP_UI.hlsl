#include "InputLayout.hlslinc"

struct InstanceData_2D
{
	float2 InstancePos :INST_POSITION;
	float2 InstanceScale :INST_SCALE;
	float4 InstanceColor :INST_COLOR;
	float4 InstanceID :INST_ID;
};

struct PixelInput_2D
{
	float4          Pos : SV_POSITION;
	float2          UV : TEXCOORD0;
	InstanceData_2D Instance : INST_DATA;
};


PixelInput_2D VS(VertexIn_2D Input, InstanceData_2D Instance)
{
	PixelInput_2D output;

	// 로컬 좌표를 NDC로 변환
	float2 scaledPosition = Input.LocalPosition * Instance.InstanceScale;
	output.Pos            = float4(scaledPosition + Instance.InstancePos, 0.5f, 1.0f);

	output.UV       = Input.UV;
	output.Instance = Instance;

	return output;
}

float4 PS(PixelInput_2D Input) : SV_TARGET
{
	return Input.Instance.InstanceID.rgba;
}

#include "InputLayout.hlslinc"
// ------------------------------------------------------------------
// : UI 전용 셰이더
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Settings
// Blend Mode : Opaque
// Shading Model : Unlit
// ------------------------------------------------------------------

Texture2D g_TextureArray[64] : register(t0);
Texture2D g_MaskTextureArray[64] : register(t64);

SamplerState g_LinearSampler : register(s0);

struct InstanceData_2D
{
	float2 InstancePos :INST_POSITION;
	float2 InstanceScale :INST_SCALE;
	float4 InstanceColor :INST_COLOR;
	int    TextureIndex :INST_TEXINDEX;
	int    MaskIndex :INST_MASKINDEX;
};

struct PixelInput_2D
{
	float4          Pos : SV_POSITION;
	float2          UV : TEXCOORD0;
	InstanceData_2D Instance : INST_DATA;
};

float4 SampleFromTextureArray(uint textureIndex, float2 uv)
{
	float4 result;
	float4 rgb;
	float4 mask;

	[branch]
	switch (textureIndex)
	{
	case 0:
		rgb = g_TextureArray[0].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[0].Sample(g_LinearSampler, uv);
		break;
	case 1:
		rgb = g_TextureArray[1].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[1].Sample(g_LinearSampler, uv);
		break;
	case 2:
		rgb = g_TextureArray[2].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[2].Sample(g_LinearSampler, uv);
		break;
	case 3:
		rgb = g_TextureArray[3].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[3].Sample(g_LinearSampler, uv);
		break;
	case 4:
		rgb = g_TextureArray[4].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[4].Sample(g_LinearSampler, uv);
		break;
	case 5:
		rgb = g_TextureArray[5].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[5].Sample(g_LinearSampler, uv);
		break;
	case 6:
		rgb = g_TextureArray[6].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[6].Sample(g_LinearSampler, uv);
		break;
	case 7:
		rgb = g_TextureArray[7].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[7].Sample(g_LinearSampler, uv);
		break;
	case 8:
		rgb = g_TextureArray[8].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[8].Sample(g_LinearSampler, uv);
		break;
	case 9:
		rgb = g_TextureArray[9].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[9].Sample(g_LinearSampler, uv);
		break;
	case 10:
		rgb = g_TextureArray[10].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[10].Sample(g_LinearSampler, uv);
		break;
	case 11:
		rgb = g_TextureArray[11].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[11].Sample(g_LinearSampler, uv);
		break;
	case 12:
		rgb = g_TextureArray[12].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[12].Sample(g_LinearSampler, uv);
		break;
	case 13:
		rgb = g_TextureArray[13].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[13].Sample(g_LinearSampler, uv);
		break;
	case 14:
		rgb = g_TextureArray[14].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[14].Sample(g_LinearSampler, uv);
		break;
	case 15:
		rgb = g_TextureArray[15].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[15].Sample(g_LinearSampler, uv);
		break;
	case 16:
		rgb = g_TextureArray[16].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[16].Sample(g_LinearSampler, uv);
		break;
	case 17:
		rgb = g_TextureArray[17].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[17].Sample(g_LinearSampler, uv);
		break;
	case 18:
		rgb = g_TextureArray[18].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[18].Sample(g_LinearSampler, uv);
		break;
	case 19:
		rgb = g_TextureArray[19].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[19].Sample(g_LinearSampler, uv);
		break;
	case 20:
		rgb = g_TextureArray[20].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[20].Sample(g_LinearSampler, uv);
		break;
	case 21:
		rgb = g_TextureArray[21].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[21].Sample(g_LinearSampler, uv);
		break;
	case 22:
		rgb = g_TextureArray[22].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[22].Sample(g_LinearSampler, uv);
		break;
	case 23:
		rgb = g_TextureArray[23].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[23].Sample(g_LinearSampler, uv);
		break;
	case 24:
		rgb = g_TextureArray[24].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[24].Sample(g_LinearSampler, uv);
		break;
	case 25:
		rgb = g_TextureArray[25].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[25].Sample(g_LinearSampler, uv);
		break;
	case 26:
		rgb = g_TextureArray[26].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[26].Sample(g_LinearSampler, uv);
		break;
	case 27:
		rgb = g_TextureArray[27].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[27].Sample(g_LinearSampler, uv);
		break;
	case 28:
		rgb = g_TextureArray[28].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[28].Sample(g_LinearSampler, uv);
		break;
	case 29:
		rgb = g_TextureArray[29].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[29].Sample(g_LinearSampler, uv);
		break;
	case 30:
		rgb = g_TextureArray[30].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[30].Sample(g_LinearSampler, uv);
		break;
	case 31:
		rgb = g_TextureArray[31].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[31].Sample(g_LinearSampler, uv);
		break;
	case 32:
		rgb = g_TextureArray[32].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[32].Sample(g_LinearSampler, uv);
		break;
	case 33:
		rgb = g_TextureArray[33].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[33].Sample(g_LinearSampler, uv);
		break;
	case 34:
		rgb = g_TextureArray[34].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[34].Sample(g_LinearSampler, uv);
		break;
	case 35:
		rgb = g_TextureArray[35].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[35].Sample(g_LinearSampler, uv);
		break;
	case 36:
		rgb = g_TextureArray[36].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[36].Sample(g_LinearSampler, uv);
		break;
	case 37:
		rgb = g_TextureArray[37].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[37].Sample(g_LinearSampler, uv);
		break;
	case 38:
		rgb = g_TextureArray[38].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[38].Sample(g_LinearSampler, uv);
		break;
	case 39:
		rgb = g_TextureArray[39].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[39].Sample(g_LinearSampler, uv);
		break;
	case 40:
		rgb = g_TextureArray[40].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[40].Sample(g_LinearSampler, uv);
		break;
	case 41:
		rgb = g_TextureArray[41].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[41].Sample(g_LinearSampler, uv);
		break;
	case 42:
		rgb = g_TextureArray[42].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[42].Sample(g_LinearSampler, uv);
		break;
	case 43:
		rgb = g_TextureArray[43].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[43].Sample(g_LinearSampler, uv);
		break;
	case 44:
		rgb = g_TextureArray[44].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[44].Sample(g_LinearSampler, uv);
		break;
	case 45:
		rgb = g_TextureArray[45].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[45].Sample(g_LinearSampler, uv);
		break;
	case 46:
		rgb = g_TextureArray[46].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[46].Sample(g_LinearSampler, uv);
		break;
	case 47:
		rgb = g_TextureArray[47].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[47].Sample(g_LinearSampler, uv);
		break;
	case 48:
		rgb = g_TextureArray[48].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[48].Sample(g_LinearSampler, uv);
		break;
	case 49:
		rgb = g_TextureArray[49].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[49].Sample(g_LinearSampler, uv);
		break;
	case 50:
		rgb = g_TextureArray[50].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[50].Sample(g_LinearSampler, uv);
		break;
	case 51:
		rgb = g_TextureArray[51].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[51].Sample(g_LinearSampler, uv);
		break;
	case 52:
		rgb = g_TextureArray[52].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[52].Sample(g_LinearSampler, uv);
		break;
	case 53:
		rgb = g_TextureArray[53].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[53].Sample(g_LinearSampler, uv);
		break;
	case 54:
		rgb = g_TextureArray[54].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[54].Sample(g_LinearSampler, uv);
		break;
	case 55:
		rgb = g_TextureArray[55].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[55].Sample(g_LinearSampler, uv);
		break;
	case 56:
		rgb = g_TextureArray[56].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[56].Sample(g_LinearSampler, uv);
		break;
	case 57:
		rgb = g_TextureArray[57].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[57].Sample(g_LinearSampler, uv);
		break;
	case 58:
		rgb = g_TextureArray[58].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[58].Sample(g_LinearSampler, uv);
		break;
	case 59:
		rgb = g_TextureArray[59].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[59].Sample(g_LinearSampler, uv);
		break;
	case 60:
		rgb = g_TextureArray[60].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[60].Sample(g_LinearSampler, uv);
		break;
	case 61:
		rgb = g_TextureArray[61].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[61].Sample(g_LinearSampler, uv);
		break;
	case 62:
		rgb = g_TextureArray[62].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[62].Sample(g_LinearSampler, uv);
		break;
	case 63:
		rgb = g_TextureArray[63].Sample(g_LinearSampler, uv);
		mask = g_MaskTextureArray[63].Sample(g_LinearSampler, uv);
		break;

	default:
		result = float4(0, 0, 0, 0);
		break;
	}

	result = rgb * mask;
	return result;
}

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
	float4 color = SampleFromTextureArray(Input.Instance.TextureIndex, Input.UV);

	color.rgb *= Input.Instance.InstanceColor.rgb;

	if (color.a < 0.5f)
		discard;
	return color;
}

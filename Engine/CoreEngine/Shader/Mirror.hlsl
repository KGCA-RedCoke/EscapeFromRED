#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"
#include "ShaderUtils.hlslinc"
// ------------------------------------------------------------------
// : Mirror (EFR게임 창문 전용)
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// Settings
// Blend Mode : Translucent (DX11 : Alpha Blending)
// Shading Model : lit
// ------------------------------------------------------------------

// 창문 더럽히는 텍스처
Texture2D    g_DirtyTexture : register(t0);
SamplerState g_SamplerLinearWrap : register(s0);

const float roughnessTile = 2.0f;
const float opacity       = 0.6f;

PixelIn_Base VS(VertexIn_Base Input, InstanceData Instance)
{
	PixelIn_Base output;

	output.Material    = Instance.Material;
	output.VertexColor = Input.VertexColor;
	output.WorldSpace  = float4(Input.Pos, 1.f);	// 로컬 좌표계
	float3 normal      = Input.Normal;

	output.WorldSpace = mul(output.WorldSpace, Instance.InstancePos);
	output.ViewDir    = normalize(WorldCameraPos.xyz - output.WorldSpace.xyz);
	output.ClipSpace  = mul(output.WorldSpace, View);
	output.ClipSpace  = mul(output.ClipSpace, Projection);
	output.TexCoord   = Input.TexCoord;
	output.Normal     = mul(normal, (float3x3)Instance.InstancePos);
	output.Tangent    = mul(Input.Tangent, (float3x3)Instance.InstancePos);
	output.Binormal   = mul(Input.Binormal, (float3x3)Instance.InstancePos);

	return output;
}

float4 PS(PixelIn_Base Input) : SV_TARGET
{
	float3 baseColor = 1.f/*Input.Material.BaseColor*/;
	float  metallic  = 1.f/*Input.Material.Metallic*/;
	float  opacity   = 0.6f;

	float4 roughnessTex = g_DirtyTexture.Sample(g_SamplerLinearWrap, Input.TexCoord * roughnessTile);
	float4 roughness    = roughnessTex * /*Input.Material.Roughness*/0.682485;

	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	float3 lightDir = normalize(DirectionalLightPos.xyz);
	float  NdotL    = saturate(dot(normalize(Input.Normal), lightDir));
	float3 diffuse  = NdotL * baseColor.rgb * DirectionalLightColor.rgb;
	float3 specular = 0;

	// 난반사광이 없으면 애초에 반사되는 색상이 없음
	if (diffuse.x > 0)
	{
		// 노말과 라이트의 반사각
		float3 reflection = reflect(lightDir, normalize(Input.Normal));
		float3 viewDir    = normalize(Input.ViewDir);

		// Specular는 카메라뷰와 반사각의 내적값을 제곱(할수록 광 나는 부분이 줄어듦) 사용
		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 1 / roughness);
	}

	// 주변광 (없으면 반사광이 없는곳은 아무것도 보이지 않음)
	float3 ambient = 0.1 * baseColor;

	for (int i = 0; i < 1; ++i)
	{
		float3 pointLight = ComputePointLight(Input.WorldSpace,
											  Input.Normal,
											  PointLight[i]);
		diffuse += pointLight;
		specular += pointLight;
	}
	float3 finalColor = float3(diffuse + ambient + (specular * metallic));
	finalColor.rgb    = lerp(finalColor.rgb, finalColor.rgb * metallic, metallic);
	float4 finalColorWithOpacity = float4(finalColor, opacity);
	return finalColorWithOpacity;
}

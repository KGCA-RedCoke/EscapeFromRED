#include "JMaterial_SkySphere.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Shader/JShader.h"
#include "Core/Window/Application.h"

JMaterial_SkySphere::JMaterial_SkySphere(JTextView InName)
	: JMaterial(InName)
{
	JMaterial_SkySphere::InitializeParams();

	// SetShader();
}

void JMaterial_SkySphere::BindMaterialPipeline(ID3D11DeviceContext*          InDeviceContext,
											   const JArray<FMaterialParam>& InInstanceParams)
{
	float gameTime = Application::s_AppInstance->GetCurrentTime();
	// Time버퍼를(상수) 넘겨줘야한다.
	mShader->UpdateConstantData(InDeviceContext, "TimeConstantBuffer", "GameTime", &gameTime);

	// 상수버퍼, 셰이더 세팅
	JMaterial::BindMaterialPipeline(InDeviceContext, InInstanceParams);

	G_Device.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int32_t slots = 0;
	G_Device.SetSamplerState(ESamplerState::LinearWrap, &slots, 1);

	// // 배경을 투평하게
	G_Device.SetBlendState(EBlendState::Opaque);
	// // 컬링 모드는 논컬링
	G_Device.SetRasterState(ERasterState::CullNone);
	// // 깊이 버퍼 읽기 쓰기 비활성화
	// G_Device.SetDepthStencilState(EDepthStencilState::DepthNone);

	ID3D11ShaderResourceView* textureArray[3];

	for (int32_t i = 0; i < 3; ++i)
	{
		if (InInstanceParams[i].TextureValue == nullptr)
		{
			textureArray[i] = nullptr;
			continue;
		}
		textureArray[i] = InInstanceParams[i].TextureValue->GetSRV();
	}

	InDeviceContext->PSSetShaderResources(0, 3, textureArray);
}

void JMaterial_SkySphere::InitializeParams()
{
	if (mMaterialParams.empty())
	{
		mMaterialParams.push_back(FMaterialParam("Cloud_01",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
		mMaterialParams.push_back(FMaterialParam("Cloud_02",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));

		mMaterialParams.push_back(FMaterialParam("Cloud_03",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
	}
}

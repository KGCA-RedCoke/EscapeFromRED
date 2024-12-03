#include "JMaterial_Window.h"

#include "Core/Graphics/XD3DDevice.h"

JMaterial_Window::JMaterial_Window(JTextView InName)
	: JMaterial(InName)
{
	JMaterial_Window::InitializeParams();
}

void JMaterial_Window::BindMaterialPipeline(ID3D11DeviceContext*          InDeviceContext,
											const JArray<FMaterialParam>& InInstanceParams)
{
	// 상수버퍼, 셰이더 세팅
	JMaterial::BindMaterialPipeline(InDeviceContext, InInstanceParams);

	G_Device.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int32_t slots = 0;
	G_Device.SetSamplerState(ESamplerState::LinearWrap, &slots, 1);

	// // 컬링 모드는 논컬링
	G_Device.SetRasterState(ERasterState::CullNone);
	// // 깊이 버퍼 읽기 쓰기 비활성화
	// G_Device.SetDepthStencilState(EDepthStencilState::DepthNone);


	ID3D11ShaderResourceView* textureArray = nullptr;
	if (InInstanceParams[0].TextureValue != nullptr)
	{
		textureArray = InInstanceParams[0].TextureValue->GetSRV();
	}

	InDeviceContext->PSSetShaderResources(0, 1, &textureArray);
}

void JMaterial_Window::InitializeParams()
{
	if (mMaterialParams.empty())
	{
		mMaterialParams.push_back(FMaterialParam("RoughnessTexture",
												 EMaterialParamValue::Texture2D,
												 &FVector::ZeroVector,
												 true));
	}

}

#include "JLandScape.h"

#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Math/Color.h"

JLandScape::JLandScape() {}

JLandScape::JLandScape(JTextView InName) {}

JLandScape::~JLandScape() {}

void JLandScape::Initialize()
{
	JActor::Initialize();
}

void JLandScape::BeginPlay()
{
	JActor::BeginPlay();
}

void JLandScape::Tick(float DeltaTime)
{
	JActor::Tick(DeltaTime);
}

void JLandScape::Destroy()
{
	JActor::Destroy();
}

void JLandScape::Draw()
{
	ID3D11DeviceContext* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

	Ptr<JCamera> currentCam = IManager.CameraManager->GetCurrentMainCam();
	assert(currentCam);

	CBuffer::Space space;
	space.Model      = XMMatrixTranspose(mWorldMat);
	space.View       = XMMatrixTranspose(currentCam->GetViewMatrix());
	space.Projection = XMMatrixTranspose(currentCam->GetProjMatrix());

	Utils::DX::UpdateDynamicBuffer(deviceContext,
								   mInstanceBuffer.CBuffer_Space[0].Get(),
								   &space,
								   sizeof(CBuffer::Space));

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	uint32_t offset = 0;
	uint32_t stride = sizeof(Vertex::FVertexInfo_Base);
	deviceContext->IASetVertexBuffers(0, 1, mInstanceBuffer.Buffer_Vertex[0].GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(mInstanceBuffer.Buffer_Index[0].Get(), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->VSSetConstantBuffers(CBuffer::SLOT_SPACE, 1, mInstanceBuffer.CBuffer_Space[0].GetAddressOf());
	deviceContext->PSSetConstantBuffers(CBuffer::SLOT_SPACE, 1, mInstanceBuffer.CBuffer_Space[0].GetAddressOf());

	IManager.CameraManager->SetCameraConstantBuffer();

	IManager.RenderManager->SetRasterState(IManager.GUIManager->IsRenderWireFrame()
											   ? ERasterState::WireFrame
											   : ERasterState::CullNone);

	IManager.RenderManager->SetBlendState(EBlendState::AlphaBlend);

	uint32_t indexCount    = mIndexInfo.size(); // 총 인덱스 수
	uint32_t instanceCount = mMapDescription.Column * mMapDescription.Row; // 인스턴스 수

	mMaterial->ApplyMaterialParams(deviceContext);

	deviceContext->DrawIndexed(indexCount, 0, 0);
	// deviceContext->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
}

bool JLandScape::Serialize_Implement(std::ofstream& FileStream)
{
	return JActor::Serialize_Implement(FileStream);
}

bool JLandScape::DeSerialize_Implement(std::ifstream& InFileStream)
{
	return JActor::DeSerialize_Implement(InFileStream);
}

void JLandScape::GenerateLandScape()
{
	mInstanceBuffer.Resize(1);

	GenVertex();
	GenIndex();

	mMaterial = IManager.MaterialManager->CreateOrLoad("Game/LandScape/Material/TestMaterial.jasset");

	if (mAlbedoMap)
	{
		FMaterialParams albedoParams;
		albedoParams.Name         = "AlbedoMap";
		albedoParams.ParamType    = EMaterialParamType::Texture2D;
		albedoParams.Key          = StringHash("AlbedoMap");
		albedoParams.Flags        = EMaterialFlag::DiffuseColor;
		albedoParams.TextureValue = mAlbedoMap;
		mMaterial->AddMaterialParam(albedoParams);
	}

	if (mNormalMap)
	{
		FMaterialParams normalParams;
		normalParams.Name         = "NormalMap";
		normalParams.ParamType    = EMaterialParamType::Texture2D;
		normalParams.Key          = StringHash("NormalMap");
		normalParams.Flags        = EMaterialFlag::NormalMap;
		normalParams.TextureValue = mNormalMap;
		mMaterial->AddMaterialParam(normalParams);
	}
}

void JLandScape::GenVertex()
{
	ID3D11Device* device = IManager.RenderManager->GetDevice();
	assert(device);

	const float halfColumn     = (mMapDescription.Column - 1) / 2.0f;
	const float halfRow        = (mMapDescription.Row - 1) / 2.0f;
	const float textureOffsetU = 1.0f / (mMapDescription.Column - 1);
	const float textureOffsetV = 1.0f / (mMapDescription.Row - 1);

	mVertexInfo.resize(mMapDescription.Row * mMapDescription.Column);
	JArray<float> rgbaData = mHeightMap->GetRGBAData();

	for (int32_t row = 0; row < mMapDescription.Row; ++row)
	{
		for (int32_t column = 0; column < mMapDescription.Column; ++column)
		{
			// 배열의 인덱스 계산 
			const int32_t index = row * mMapDescription.Column + column;

			// 페이스 노말
			// FVector faceNormal = Utils::DX::ComputeFaceNormal()

			// 버텍스 정보 설정
			mVertexInfo[index].Position = FVector(
												  (column - halfColumn) * mMapDescription.CellDistance,
												  rgbaData[index] * mMapDescription.ScaleHeight,
												  -(row - halfRow) * mMapDescription.CellDistance);

			mVertexInfo[index].UV = FVector2(column * textureOffsetU,
											 row * textureOffsetV);
			mVertexInfo[index].Normal   = FVector(0.0f, 1.0f, 0.0f);
			mVertexInfo[index].Binormal = FVector(1.0f, 0.0f, 0.0f);
			mVertexInfo[index].Tangent  = FVector(0.0f, 0.0f, 1.0f);
			mVertexInfo[index].Color    = FLinearColor::Green;
		}
	}

	Utils::DX::CreateBuffer(device,
							D3D11_BIND_VERTEX_BUFFER,
							reinterpret_cast<void**>(&mVertexInfo.at(0)),
							sizeof(Vertex::FVertexInfo_Base),
							mVertexInfo.size(),
							mInstanceBuffer.Buffer_Vertex[0].GetAddressOf());
}

void JLandScape::GenIndex()
{
	ID3D11Device* device = IManager.RenderManager->GetDevice();
	assert(device);

	const int32_t cellRow = (mMapDescription.Column - 1);
	const int32_t cellCol = (mMapDescription.Row - 1);

	mIndexInfo.resize(cellRow * cellCol * 2 * 3);

	int32_t index = 0;
	for (int32_t row = 0; row < cellRow; ++row)
	{
		for (int32_t column = 0; column < cellCol; ++column)
		{
			// 1 (clockwise)
			mIndexInfo[index++] = row * mMapDescription.Column + column;
			mIndexInfo[index++] = row * mMapDescription.Column + column + 1;
			mIndexInfo[index++] = (row + 1) * mMapDescription.Column + column;

			// 2 (clockwise)
			mIndexInfo[index++] = mIndexInfo[index - 1];
			mIndexInfo[index++] = mIndexInfo[index - 3];
			mIndexInfo[index++] = (row + 1) * mMapDescription.Column + column + 1;
		}
	}

	Utils::DX::CreateBuffer(device,
							D3D11_BIND_INDEX_BUFFER,
							reinterpret_cast<void**>(&mIndexInfo.at(0)),
							sizeof(uint32_t),
							mIndexInfo.size(),
							mInstanceBuffer.Buffer_Index[0].GetAddressOf());


	Utils::DX::CreateBuffer(device,
							D3D11_BIND_CONSTANT_BUFFER,
							nullptr,
							sizeof(CBuffer::Space),
							1,
							mInstanceBuffer.CBuffer_Space[0].GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE);
}

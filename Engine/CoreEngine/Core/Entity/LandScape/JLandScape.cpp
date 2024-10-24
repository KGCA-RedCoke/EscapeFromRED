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

	mMaterial->BindMaterialPipeline(deviceContext);

	deviceContext->DrawIndexed(indexCount, 0, 0);
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
	GenFaceNormal();
	GenVertexNormal();
	GenBuffer();

	mMaterial = IManager.MaterialManager->CreateOrLoad("Game/LandScape/Material/TestMaterial.jasset");

	if (mAlbedoMap)
	{
		FMaterialParam albedoParams;
		albedoParams.ParamValue   = EMaterialParamValue::Texture2D;
		albedoParams.TextureValue = mAlbedoMap;
		mMaterial->AddMaterialParam(albedoParams);
	}

	if (mNormalMap)
	{
		FMaterialParam normalParams;
		normalParams.ParamValue   = EMaterialParamValue::Texture2D;
		normalParams.TextureValue = mNormalMap;
		mMaterial->AddMaterialParam(normalParams);
	}
}

void JLandScape::GenVertex()
{
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

			mVertexInfo[index].Color = FLinearColor::Green;
		}
	}
}

void JLandScape::GenIndex()
{
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
			mIndexInfo[index++] = (row + 1) * mMapDescription.Column + (column + 1);
		}
	}
}

void JLandScape::GenFaceNormal()
{
	const int32_t cellRow = (mMapDescription.Column - 1);
	const int32_t cellCol = (mMapDescription.Row - 1);

	mNormalInfo.resize(cellRow * cellCol * 2);
	mIndexGroup.resize(mMapDescription.Row * mMapDescription.Column);

	for (int32_t face = 0; face < mIndexGroup.size(); ++face)
	{
		uint32_t index0 = mIndexInfo[face * 3 + 0];
		uint32_t index1 = mIndexInfo[face * 3 + 1];
		uint32_t index2 = mIndexInfo[face * 3 + 2];

		FVector v0 = mVertexInfo[index0].Position;
		FVector v1 = mVertexInfo[index1].Position;
		FVector v2 = mVertexInfo[index2].Position;

		FVector edge1 = v1 - v0;
		FVector edge2 = v2 - v0;

		mNormalInfo[face] = edge1.Cross(edge2);
		mNormalInfo[face].Normalize();

		mIndexGroup[index0].push_back(face);
		mIndexGroup[index1].push_back(face);
		mIndexGroup[index2].push_back(face);
	}
}

void JLandScape::GenVertexNormal()
{
	for (int32_t vertex = 0; vertex < mVertexInfo.size(); ++vertex)
	{
		FVector normal = FVector::ZeroVector;

		for (int32_t face : mIndexGroup[vertex])
		{
			normal += mNormalInfo[face];
		}

		normal.Normalize();
		mVertexInfo[vertex].Normal = normal;
	}
}

void JLandScape::GenBuffer()
{
	ID3D11Device* device = IManager.RenderManager->GetDevice();
	assert(device);


	Utils::DX::CreateBuffer(device,
							D3D11_BIND_VERTEX_BUFFER,
							reinterpret_cast<void**>(&mVertexInfo.at(0)),
							sizeof(Vertex::FVertexInfo_Base),
							mVertexInfo.size(),
							mInstanceBuffer.Buffer_Vertex[0].GetAddressOf());

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

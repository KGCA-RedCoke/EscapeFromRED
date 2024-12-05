#include "JLandScape.h"

#include "Core/Graphics/ShaderStructs.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Material/MMaterialInstanceManager.h"
#include "Core/Graphics/Material/Instance/JMaterialInstance.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/Math/Color.h"
#include "GUI/MGUIManager.h"

JLandScape::JLandScape() {}

JLandScape::JLandScape(JTextView InName) {}

JLandScape::~JLandScape() {}

void JLandScape::Initialize()
{
	AActor::Initialize();
}

void JLandScape::BeginPlay()
{
	AActor::BeginPlay();
}

void JLandScape::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
}

void JLandScape::Destroy()
{
	AActor::Destroy();
}

void JLandScape::Draw()
{
	ID3D11DeviceContext* deviceContext = GetWorld.D3D11API->GetImmediateDeviceContext();
	assert(deviceContext);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	uint32_t offset = 0;
	uint32_t stride = sizeof(Vertex::FVertexInfo_Base);
	deviceContext->IASetVertexBuffers(0, 1, mInstanceBuffer.Buffer_Vertex[0].GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(mInstanceBuffer.Buffer_Index[0].Get(), DXGI_FORMAT_R32_UINT, 0);

	GetWorld.D3D11API->SetRasterState(GetWorld.GUIManager->IsRenderWireFrame()
											   ? ERasterState::WireFrame
											   : ERasterState::CullNone);

	GetWorld.D3D11API->SetBlendState(EBlendState::AlphaBlend);


	mMaterial->BindMaterial(deviceContext);

	const uint32_t indexCount = mIndexInfo.size(); // 총 인덱스 수
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

bool JLandScape::Serialize_Implement(std::ofstream& FileStream)
{
	return AActor::Serialize_Implement(FileStream);
}

bool JLandScape::DeSerialize_Implement(std::ifstream& InFileStream)
{
	return AActor::DeSerialize_Implement(InFileStream);
}

void JLandScape::GenerateLandScape()
{
	mInstanceBuffer.Resize(1);

	GenVertex();
	GenIndex();
	GenFaceNormal();
	GenVertexNormal();
	GenBuffer();

	mMaterial = GetWorld.MaterialInstanceManager->Load("Game/Materials/DefaultMaterial.jasset");

	if (mAlbedoMap)
	{
		FMaterialParam albedoParams;
		albedoParams.Name         = CBuffer::NAME_CONSTANT_VARIABLE_MATERIAL_DIFFUSE;
		albedoParams.ParamValue   = EMaterialParamValue::Texture2D;
		albedoParams.TextureValue = mAlbedoMap;
		
	}

	// if (mNormalMap)
	// {
	// 	FMaterialParam normalParams;
	// 	normalParams.ParamValue   = EMaterialParamValue::Texture2D;
	// 	normalParams.TextureValue = mNormalMap;
	// 	mMaterial->AddMaterialParam(normalParams);
	// }
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
	/// 삼각형의 노말벡터를 어떻게 구할 수 있을까?
	/// Step 1. 삼각형 내의 위치 벡터를 구한다.
	/// Step 2. 위치 벡터의 뺄셈을 이용해 두 벡터(엣지)를 구한다.
	/// Step 3. 두 엣지에서 외적(Cross Product)을 이용해 노말 벡터를 구한다.

	const int32_t cellRow       = (mMapDescription.Column - 1);
	const int32_t cellCol       = (mMapDescription.Row - 1);
	const int32_t triangleCount = cellRow * cellCol * 2; // 삼각형 갯수

	mFaceInfo.resize(triangleCount);	// 삼각형 갯수만큼의 페이스 노말을 생성
	mVertexGroup.resize(mMapDescription.Row * mMapDescription.Column);	// 정점 갯수 만큼의 벡터 그룹을 생성

	for (int32_t face = 0; face < triangleCount; ++face)
	{
		// 삼각형에 있는 세 정점의 인덱스 조회
		uint32_t index0 = mIndexInfo[face * 3 + 0];
		uint32_t index1 = mIndexInfo[face * 3 + 1];
		uint32_t index2 = mIndexInfo[face * 3 + 2];

		// 인덱스별 정점위치 조회
		FVector v0 = mVertexInfo[index0].Position;
		FVector v1 = mVertexInfo[index1].Position;
		FVector v2 = mVertexInfo[index2].Position;

		// 벡터 뺄셈을 통해 두 벡터(엣지)를 구함
		FVector edge1 = v1 - v0;
		FVector edge2 = v2 - v0;

		// 두 벡터의 외적을 통해 노말 벡터를 구함 + 정규화
		mFaceInfo[face] = edge1.Cross(edge2);
		mFaceInfo[face].Normalize();

		// 정점 그룹에 페이스 인덱스 추가
		mVertexGroup[index0].push_back(face);
		mVertexGroup[index1].push_back(face);
		mVertexGroup[index2].push_back(face);
	}
}

void JLandScape::GenVertexNormal()
{
	for (int32_t vertex = 0; vertex < mVertexInfo.size(); ++vertex)
	{
		for (int32_t normalCount = 0; normalCount < mVertexGroup[vertex].size(); ++normalCount)
		{
			const int32_t faceIndex = mVertexGroup[vertex][normalCount];
			mVertexInfo[vertex].Normal += mFaceInfo[faceIndex];
		}
		mVertexInfo[vertex].Normal.Normalize();
	}
}

void JLandScape::GenBuffer()
{
	ID3D11Device* device = GetWorld.D3D11API->GetDevice();
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

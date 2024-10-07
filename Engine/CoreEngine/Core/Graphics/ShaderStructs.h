#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/FileIO/JSerialization.h"
#include "Core/Utils/Graphics/DXUtils.h"
#include "Core/Utils/Math/Vector4.h"

/**
 * @brief 셰이더에서 사용하는 상수 버퍼 구조체
 * 상수 버퍼는 될 수 있으면 16바이트 단위로 정렬 시키는 것이 좋다.
 * hlsl에서 12바이트를 사용하면 16바이트로 정렬되어 4바이트가 낭비된다.
 *  그러니까 12바이트 뒤에 12바이트가 또 오면 4바이트 패딩을 넣어 16바이트로 정렬시킨다.
 *  만약 12바이트를 연속적으로 넣으면 데이터를 잘못 읽어올 수 있다.
 */
namespace CBuffer
{
	/**
	 * Model, View, Projection
	 */
	struct Space
	{
		FMatrix Model;
		FMatrix View;
		FMatrix Projection;
	};

	struct Light
	{
		FVector4 LightPos;
		FVector4 LightColor;
	};

	struct Time
	{
		FVector4 WorldTime;
	};

	struct Camera
	{
		FVector4 CameraPos;
	};

	// FVector(12)에 float값 하나 연결하면 딱 맞긴 한데...
	struct Material
	{
		// sizeof(Fvector)(16) * 8 = 128
		FVector4 DiffuseColor;
		FVector4 EmissiveColor;
		FVector4 SpecularColor;
		FVector4 ReflectionColor;
		FVector4 AmbientColor;
		FVector4 TransparentColor;
		FVector4 DisplacementColor;
		FVector4 NormalMap;

		// sizeof(float) * 9 = 36
		float DiffuseFactor;
		float EmissiveFactor;
		float SpecularFactor;
		float ReflectionFactor;
		float AmbientFactor;
		float TransparentFactor;
		float DisplacementFactor;
		float Shininess;
		float Opacity;

		// sizeof(BOOL) * 7 = 28
		int32_t bUseDiffuseMap;
		int32_t bUseEmissiveMap;
		int32_t bUseSpecularMap;
		int32_t bUseReflectionMap;
		int32_t bUseAmbientMap;
		int32_t bUseDisplacementMap;
		int32_t bUseNormalMap;

		// total = 128 + 36 + 28 = 192
	};
}

/**
 * @brief 정점 정보 구조체
 */
namespace Vertex
{
	struct FVertexInfo_ScreenQuad
	{
		FVector  Position = FVector::ZeroVector;
		FVector2 UV       = FVector2::ZeroVector;
	};

	struct FVertexInfo_2D
	{
		FVector  Position;
		FVector2 UV;
		FVector4 Color;


		/// 구조체에 vtable이 없어야 하므로 가상함수 사용X (정점 버퍼에 넘길 때 크기가 다르면 문제가 생길 수 있음)
		void Serialize_Implement(std::ofstream& FileStream)
		{
			// Position
			FileStream.write(reinterpret_cast<char*>(&Position), sizeof(Position));
			// UV
			FileStream.write(reinterpret_cast<char*>(&UV), sizeof(UV));
			// Color
			FileStream.write(reinterpret_cast<char*>(&Color), sizeof(Color));
		}

		void DeSerialize_Implement(std::ifstream& InFileStream)
		{
			// Position
			InFileStream.read(reinterpret_cast<char*>(&Position), sizeof(Position));
			// UV
			InFileStream.read(reinterpret_cast<char*>(&UV), sizeof(UV));
			// Color
			InFileStream.read(reinterpret_cast<char*>(&Color), sizeof(Color));
		}
	};

	struct FVertexInfo_Base : public FVertexInfo_2D
	{
		FVector Normal;
		FVector Tangent;
		FVector Binormal;

		void Serialize_Implement(std::ofstream& FileStream)
		{
			using Base = FVertexInfo_2D;
			Base::Serialize_Implement(FileStream);

			// NormalMap
			FileStream.write(reinterpret_cast<char*>(&Normal), sizeof(Normal));
			// Tangent
			FileStream.write(reinterpret_cast<char*>(&Tangent), sizeof(Tangent));
			// Binormal
			FileStream.write(reinterpret_cast<char*>(&Binormal), sizeof(Binormal));
		}

		void DeSerialize_Implement(std::ifstream& InFileStream)
		{
			using Base = FVertexInfo_2D;
			Base::DeSerialize_Implement(InFileStream);

			// NormalMap
			InFileStream.read(reinterpret_cast<char*>(&Normal), sizeof(Normal));
			// Tangent
			InFileStream.read(reinterpret_cast<char*>(&Tangent), sizeof(Tangent));
			// Binormal
			InFileStream.read(reinterpret_cast<char*>(&Binormal), sizeof(Binormal));
		}
	};

}

/**
 * @brief Basic Buffer 구조체
 */
namespace Buffer
{
	/**
	 * 상수 버퍼 묶음 (기본이 되는 버퍼)
	 */
	struct FBufferSpaceLightTime
	{
		// --------------------- 버퍼 ---------------------
		ComPtr<ID3D11Buffer> CBuffer_Space;	// World, View, Projection
		ComPtr<ID3D11Buffer> CBuffer_Light;	// Light Direction
		ComPtr<ID3D11Buffer> CBuffer_Time;		// Time

		// --------------------- 실제 버퍼에 넘길 데이터 ---------------------
		CBuffer::Space Space;
		CBuffer::Light Light;
		CBuffer::Time  Time;

		void CreateBuffer(ID3D11Device* InDevice)
		{
			// 상수 버퍼 생성 (WVP)
			Utils::DX::CreateBuffer(InDevice,
									D3D11_BIND_CONSTANT_BUFFER,
									nullptr,
									sizeof(CBuffer::Space),
									1,
									CBuffer_Space.GetAddressOf(),
									D3D11_USAGE_DYNAMIC,
									D3D11_CPU_ACCESS_WRITE);


			// 상수 버퍼 생성 (World Directional Light)
			Utils::DX::CreateBuffer(InDevice,
									D3D11_BIND_CONSTANT_BUFFER,
									nullptr,
									sizeof(CBuffer::Light),
									1,
									CBuffer_Light.GetAddressOf(),
									D3D11_USAGE_DYNAMIC,
									D3D11_CPU_ACCESS_WRITE);

			// 상수 버퍼 생성 (Time)
			Utils::DX::CreateBuffer(InDevice,
									D3D11_BIND_CONSTANT_BUFFER,
									nullptr,
									sizeof(CBuffer::Time),
									1,
									CBuffer_Time.GetAddressOf(),
									D3D11_USAGE_DYNAMIC,
									D3D11_CPU_ACCESS_WRITE);
		}

		void UpdateSpace(ID3D11DeviceContext* InDeviceContext, const FMatrix& InModel, const FMatrix& InView,
						 const FMatrix&       InProjection)
		{
			Space.Model      = InModel;
			Space.View       = InView;
			Space.Projection = InProjection;

			Utils::DX::UpdateDynamicBuffer(InDeviceContext,
										   CBuffer_Space.Get(),
										   &Space,
										   sizeof(CBuffer::Space));
		}

		void UpdateLight(ID3D11DeviceContext* InDeviceContext, const FVector4& InLightPos, const FVector4& InLightColor)
		{
			Light.LightPos   = InLightPos;
			Light.LightColor = InLightColor;

			Utils::DX::UpdateDynamicBuffer(InDeviceContext,
										   CBuffer_Light.Get(),
										   &Light,
										   sizeof(CBuffer::Light));
		}

		void UpdateTime(ID3D11DeviceContext* InDeviceContext, const FVector4& InWorldTime)
		{
			Time.WorldTime = InWorldTime;

			Utils::DX::UpdateDynamicBuffer(InDeviceContext,
										   CBuffer_Time.Get(),
										   &Time,
										   sizeof(CBuffer::Time));
		}

		void PreRender(ID3D11DeviceContext* InDeviceContext)
		{
			// --------------------- Buffer Space ---------------------
			InDeviceContext->VSSetConstantBuffers(0, 1, CBuffer_Space.GetAddressOf());
			InDeviceContext->PSSetConstantBuffers(0, 1, CBuffer_Space.GetAddressOf());

			// --------------------- Buffer Light ---------------------
			InDeviceContext->VSSetConstantBuffers(1, 1, CBuffer_Light.GetAddressOf());
			InDeviceContext->PSSetConstantBuffers(1, 1, CBuffer_Light.GetAddressOf());

			// --------------------- Buffer Time ---------------------
			InDeviceContext->VSSetConstantBuffers(3, 1, CBuffer_Time.GetAddressOf());
			InDeviceContext->PSSetConstantBuffers(3, 1, CBuffer_Time.GetAddressOf());
		}

	};

	struct FBufferInstance
	{
		std::vector<ComPtr<ID3D11Buffer>>  Buffer_Vertex;			// Vertex 
		std::vector<ComPtr<ID3D11Buffer>>  Buffer_Index;			// Index
		std::vector<FBufferSpaceLightTime> CBuffer_SpaceLightTime;	// Space, Light, Time


		void Resize(const int32_t Size)
		{
			Buffer_Vertex.clear();
			Buffer_Index.clear();
			CBuffer_SpaceLightTime.clear();

			Buffer_Vertex.resize(Size);
			Buffer_Index.resize(Size);
			CBuffer_SpaceLightTime.resize(Size);
		}
	};


}

/**
 * 삼각형(정점)을 나타내는 데이터 구조체
 * @tparam T 정점 구조체
 */
template <typename T>
struct FTriangle
{
	static_assert(std::is_base_of_v<Vertex::FVertexInfo_2D, T>, "T must be derived from FVertexInfo_2D");

	T       Vertex[3];						// 0, 1, 2 
	FVector Normal   = FVector::ZeroVector;	// NormalMap
	int32_t SubIndex = -1;					// SubIndex

public:
	FTriangle() = default;

	FTriangle(int32_t InIndex)
		: SubIndex(InIndex) {};

};

/**
 * 비교 연산자 (람다보다 깔끔한듯?)
 * @tparam T 정점 정보 
 */
template <typename T>
struct IsTriangleSame
{
	FTriangle<T> Tri;

	IsTriangleSame(FTriangle<T> Data)
		: Tri(Data) {}

	bool operator()(FTriangle<T>& Value)
	{
		return Value.SubIndex == Tri.SubIndex;
	}
};

template <typename T>
struct IsVertexSame
{
	static_assert(std::is_base_of_v<Vertex::FVertexInfo_2D, T>, "T must be derived from FVertexInfo_2D");

	T Vertex;

	IsVertexSame(T Data)
		: Vertex(Data) {}

	bool operator()(T& Value)
	{
		return Value.Position == Vertex.Position && Value.UV == Vertex.UV && Value.Color == Vertex.Color;
	}
};

/**
 * 
 * @tparam T 정점 정보
 */
template <typename T>
struct JVertexData final : public ISerializable
{
	static_assert(std::is_base_of_v<Vertex::FVertexInfo_2D, T>, "T must be derived from FVertexInfo_2D");

	int32_t                   FaceCount = 0;
	std::vector<T>            VertexArray;
	std::vector<uint32_t>     IndexArray;
	std::vector<FTriangle<T>> TriangleList;

	uint32_t GetType() const override
	{
		return StringHash("VertexData");
	}

	bool Serialize_Implement(std::ofstream& FileStream) override
	{
		if (!Utils::Serialization::SerializeMetaData(FileStream, this))
		{
			return false;
		}

		// FaceCount
		Utils::Serialization::Serialize_Primitive(&FaceCount, sizeof(FaceCount), FileStream);

		// VertexArray
		size_t vertexSize = VertexArray.size();
		Utils::Serialization::Serialize_Primitive(&vertexSize, sizeof(vertexSize), FileStream);
		for (int32_t i = 0; i < vertexSize; ++i)
		{
			VertexArray[i].Serialize_Implement(FileStream);
		}

		// IndexArray
		size_t indexSize = IndexArray.size();
		Utils::Serialization::Serialize_Primitive(&indexSize, sizeof(indexSize), FileStream);
		for (int32_t i = 0; i < indexSize; ++i)
		{
			Utils::Serialization::Serialize_Primitive(&IndexArray[i], sizeof(IndexArray[i]), FileStream);
		}

		return true;
	}

	bool DeSerialize_Implement(std::ifstream& InFileStream) override
	{
		JAssetMetaData metaData;
		if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
		{
			return false;
		}

		// FaceCount
		Utils::Serialization::DeSerialize_Primitive(&FaceCount, sizeof(FaceCount), InFileStream);

		// VertexArray
		size_t vertexSize;
		Utils::Serialization::DeSerialize_Primitive(&vertexSize, sizeof(vertexSize), InFileStream);
		VertexArray.resize(vertexSize);
		for (int32_t i = 0; i < vertexSize; ++i)
		{
			VertexArray[i].DeSerialize_Implement(InFileStream);
		}

		// IndexArray
		size_t indexSize;
		Utils::Serialization::DeSerialize_Primitive(&indexSize, sizeof(indexSize), InFileStream);
		IndexArray.resize(indexSize);
		for (int32_t i = 0; i < indexSize; ++i)
		{
			Utils::Serialization::DeSerialize_Primitive(&IndexArray[i], sizeof(IndexArray[i]), InFileStream);
		}

		return true;
	}


	/**
	 * 삼각형들을 순회하면서 IndexArray를 생성한다.
	 * FIXME: 중복된 정점을 처리할 수 있도록 수정해야 한다.
	 * @param TriangleList Polygon(삼각형)리스트
	 * @param Material 머티리얼 인덱스(SubMesh) 
	 * @param StartTriangle 삼각형 시작 인덱스
	 * @return 
	 */
	int32_t GenerateIndexArray(std::vector<FTriangle<T>>& TriangleList, int32_t Material = -1,
							   int32_t                    StartTriangle                  = 0)
	{
		int32_t faceNum = TriangleList.size();

		VertexArray.reserve(faceNum * 3);
		IndexArray.reserve(faceNum * 3);

		// SubMesh 사용시 Check
		FTriangle<T> triSame(Material);
		if (Material >= 0)
		{
			faceNum = std::count_if(TriangleList.begin(), TriangleList.end(), IsTriangleSame<T>(triSame));
		}

		for (int32_t face = 0; face < faceNum; ++face)
		{
			for (int32_t i = 0; i < 3; ++i)
			{
				auto vertex = TriangleList[StartTriangle + face].Vertex[i];

				auto foundedVertex = std::find_if(VertexArray.begin(), VertexArray.end(), IsVertexSame<T>(vertex));
				if (foundedVertex != VertexArray.end())
				{
					int32_t vertexIndex = std::distance(VertexArray.begin(), foundedVertex); // 찾은 것의 Index가 필요
					IndexArray.push_back(vertexIndex);									// IndexArray에 추가
				}
				else
				{
					VertexArray.push_back(vertex);
					IndexArray.push_back(VertexArray.size() - 1);
				}
			}
		}
		return faceNum;
	}
};

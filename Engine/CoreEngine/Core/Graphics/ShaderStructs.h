#pragma once
#include "GraphicDevice.h"
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/FileIO/JSerialization.h"
#include "Core/Utils/Math/Vector4.h"

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
		FVector4 CameraPos;
		FVector4 LightPos;
		FVector4 LightColor;
	};

	struct Time
	{
		FVector4 WorldTime;
	};

	struct Camera
	{
		FVector4 CamPos;
	};
}

namespace Vertex
{
	struct FVertexInfo_2D : public ISerializable
	{
		FVector  Position;
		FVector2 UV;
		FVector4 Color;

		void Serialize(std::ofstream& FileStream) override
		{
			// Position
			FileStream.write(reinterpret_cast<char*>(&Position), sizeof(Position));
			// UV
			FileStream.write(reinterpret_cast<char*>(&UV), sizeof(UV));
			// Color
			FileStream.write(reinterpret_cast<char*>(&Color), sizeof(Color));
		}

		void DeSerialize(std::ifstream& InFileStream) override
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

		void Serialize(std::ofstream& FileStream) override
		{
			using Base = FVertexInfo_2D;

			Base::Serialize(FileStream);

			// Normal
			FileStream.write(reinterpret_cast<char*>(&Normal), sizeof(Normal));
			// Tangent
			FileStream.write(reinterpret_cast<char*>(&Tangent), sizeof(Tangent));
			// Binormal
			FileStream.write(reinterpret_cast<char*>(&Binormal), sizeof(Binormal));
		}

		void DeSerialize(std::ifstream& InFileStream) override
		{
			using Base = FVertexInfo_2D;

			Base::DeSerialize(InFileStream);

			// Normal
			InFileStream.read(reinterpret_cast<char*>(&Normal), sizeof(Normal));
			// Tangent
			InFileStream.read(reinterpret_cast<char*>(&Tangent), sizeof(Tangent));
			// Binormal
			InFileStream.read(reinterpret_cast<char*>(&Binormal), sizeof(Binormal));
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
	FVector Normal   = FVector::ZeroVector;	// Normal
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
struct JData : public ISerializable
{
	static_assert(std::is_base_of_v<T, Vertex::FVertexInfo_2D>, "T must be derived from FVertexInfo_2D");

	int32_t                   FaceCount = 0;
	std::vector<T>            VertexArray;
	std::vector<WORD>         IndexArray;
	std::vector<FTriangle<T>> TriangleList;

	void Serialize(std::ofstream& FileStream) override
	{
		// FaceCount
		FileStream.write(reinterpret_cast<char*>(&FaceCount), sizeof(FaceCount));

		// VertexArray
		size_t vertexSize = VertexArray.size();
		FileStream.write(reinterpret_cast<char*>(&vertexSize), sizeof(vertexSize));
		for (int32_t i = 0; i < vertexSize; ++i)
		{
			VertexArray[i].Serialize(FileStream);
		}

		// IndexArray
		size_t indexSize = IndexArray.size();
		FileStream.write(reinterpret_cast<char*>(&indexSize), sizeof(indexSize));
		for (int32_t i = 0; i < indexSize; ++i)
		{
			FileStream.write(reinterpret_cast<char*>(&IndexArray[i]), sizeof(IndexArray[i]));
		}
	}

	void DeSerialize(std::ifstream& InFileStream) override
	{
		// FaceCount
		InFileStream.read(reinterpret_cast<char*>(&FaceCount), sizeof(FaceCount));

		// VertexArray
		size_t vertexSize;
		InFileStream.read(reinterpret_cast<char*>(&vertexSize), sizeof(vertexSize));
		VertexArray.resize(vertexSize);
		for (int32_t i = 0; i < vertexSize; ++i)
		{
			VertexArray[i].DeSerialize(InFileStream);
		}

		// IndexArray
		size_t indexSize;
		InFileStream.read(reinterpret_cast<char*>(&indexSize), sizeof(indexSize));
		IndexArray.resize(indexSize);
		for (int32_t i = 0; i < indexSize; ++i)
		{
			InFileStream.read(reinterpret_cast<char*>(&IndexArray[i]), sizeof(IndexArray[i]));
		}
	}


	/**
	 * 삼각형들을 순회하면서 IndexArray를 생성한다.
	 * FIXME: 중복된 정점을 처리할 수 있도록 수정해야 한다.
	 * @param TriangleList Polygon(삼각형)리스트
	 * @param Material 머티리얼 인덱스(SubMesh) 
	 * @param StartTriangle 삼각형 시작 인덱스
	 * @return 
	 */
	int32_t GenerateIndexArray(std::vector<FTriangle<T>>& TriangleList, int32_t Material = -1, int32_t StartTriangle = 0)
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

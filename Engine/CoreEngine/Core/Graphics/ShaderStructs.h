﻿#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/FileIO/JSerialization.h"
#include "Core/Utils/Math/Vector4.h"

constexpr uint32_t SIZE_INDEX_BUFFER = sizeof(uint32_t);
constexpr uint32_t SIZE_MAX_BONE_NUM = 255;

/**
 * @brief 셰이더에서 사용하는 상수 버퍼 구조체
 * 상수 버퍼는 될 수 있으면 16바이트 단위로 정렬 시키는 것이 좋다.
 * hlsl에서 12바이트를 사용하면 16바이트로 정렬되어 4바이트가 낭비된다.
 *  그러니까 12바이트 뒤에 12바이트가 또 오면 4바이트 패딩을 넣어 16바이트로 정렬시킨다.
 *  만약 12바이트를 연속적으로 넣으면 데이터를 잘못 읽어올 수 있다.
 */
namespace CBuffer
{
	constexpr const char* NAME_CONSTANT_BUFFER_SPACE                 = "ModelViewProjectionConstantBuffer";
	constexpr const char* NAME_CONSTANT_BUFFER_LIGHT                 = "LightConstantBuffer";
	constexpr const char* NAME_CONSTANT_BUFFER_CAMERA                = "CameraConstantBuffer";
	constexpr const char* NAME_CONSTANT_BUFFER_TIME                  = "WorldTimeConstantBuffer";
	constexpr const char* NAME_CONSTANT_BUFFER_MATERIAL              = "BasicMaterialConstantBuffer";
	constexpr const char* NAME_CONSTANT_BUFFER_COLOR_ID              = "ColorIDConstantBuffer";
	constexpr const char* NAME_CONSTANT_VARIABLE_SPACE_WORLD         = "World";
	constexpr const char* NAME_CONSTANT_VARIABLE_SPACE_VIEW          = "View";
	constexpr const char* NAME_CONSTANT_VARIABLE_SPACE_PROJ          = "Projection";
	constexpr const char* NAME_CONSTANT_VARIABLE_LIGHT_POS           = "DirectionalLightPos";
	constexpr const char* NAME_CONSTANT_VARIABLE_LIGHT_COLOR         = "DirectionalLightColor";
	constexpr const char* NAME_CONSTANT_VARIABLE_CAMERA_POS          = "WorldCameraPos";
	constexpr const char* NAME_CONSTANT_VARIABLE_MATERIAL_DIFFUSE    = "Diffuse";
	constexpr const char* NAME_CONSTANT_VARIABLE_MATERIAL_NORMAL     = "Normal";
	constexpr const char* NAME_CONSTANT_VARIABLE_MATERIAL_AMBIENT    = "Ambient";
	constexpr const char* NAME_CONSTANT_VARIABLE_MATERIAL_AO         = "AmbientOcclusion";
	constexpr const char* NAME_CONSTANT_VARIABLE_MATERIAL_ROUGHNESS  = "Roughness";
	constexpr const char* NAME_CONSTANT_VARIABLE_MATERIAL_METALLIC   = "Metallic";
	constexpr const char* NAME_CONSTANT_VARIABLE_MATERIAL_USAGE_FLAG = "TextureUsageFlag";

	const uint32_t HASH_CONSTANT_BUFFER_SPACE                 = StringHash(NAME_CONSTANT_BUFFER_SPACE);
	const uint32_t HASH_CONSTANT_BUFFER_LIGHT                 = StringHash(NAME_CONSTANT_BUFFER_LIGHT);
	const uint32_t HASH_CONSTANT_BUFFER_CAMERA                = StringHash(NAME_CONSTANT_BUFFER_CAMERA);
	const uint32_t HASH_CONSTANT_BUFFER_TIME                  = StringHash(NAME_CONSTANT_BUFFER_TIME);
	const uint32_t HASH_CONSTANT_BUFFER_MATERIAL              = StringHash(NAME_CONSTANT_BUFFER_MATERIAL);
	const uint32_t HASH_CONSTANT_VARIABLE_SPACE_WORLD         = StringHash(NAME_CONSTANT_VARIABLE_SPACE_WORLD);
	const uint32_t HASH_CONSTANT_VARIABLE_SPACE_VIEW          = StringHash(NAME_CONSTANT_VARIABLE_SPACE_VIEW);
	const uint32_t HASH_CONSTANT_VARIABLE_SPACE_PROJ          = StringHash(NAME_CONSTANT_VARIABLE_SPACE_PROJ);
	const uint32_t HASH_CONSTANT_VARIABLE_LIGHT_POS           = StringHash(NAME_CONSTANT_VARIABLE_LIGHT_POS);
	const uint32_t HASH_CONSTANT_VARIABLE_LIGHT_COLOR         = StringHash(NAME_CONSTANT_VARIABLE_LIGHT_COLOR);
	const uint32_t HASH_CONSTANT_VARIABLE_CAMERA_POS          = StringHash(NAME_CONSTANT_VARIABLE_CAMERA_POS);
	const uint32_t HASH_CONSTANT_VARIABLE_MATERIAL_USAGE_FLAG = StringHash(NAME_CONSTANT_VARIABLE_MATERIAL_USAGE_FLAG);

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

	enum ETextureUsage : uint32_t
	{
		None             = 0,
		Diffuse          = 1 << 0,
		Normal           = 1 << 1,
		AmbientOcclusion = 1 << 2,
		Roughness        = 1 << 3,
		Metallic         = 1 << 4,
	};

	constexpr const char* TextureUsageString[]
	{
		"None",
		"Diffuse",
		"Normal",
		"AmbientOcclusion",
		"Roughness",
		"Metallic"
	};

	struct FMaterialColorParam
	{
		FVector Color;
		float   Factor;
	};

	struct FMaterialValueParam
	{
		float Value;
		float Factor;
	};

	struct Material_Basic
	{
		FMaterialColorParam Diffuse;	// 16 Bytes
		FMaterialColorParam Ambient;	// 16 Bytes

		FMaterialValueParam AmbientOcclusion;	// 8 Bytes 
		FMaterialValueParam Roughness;			// 8 Bytes
		FMaterialValueParam Metallic; 			// 8 Bytes

		uint32_t TextureUsageFlag = 0;	// 4 Bytes

		float Padding;						// 4 Bytes
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
		FVector  Normal;
		FVector  Tangent;
		FVector  Binormal;
		FVector4 BoneIndices = FVector4::ZeroVector;
		FVector4 BoneWeights = FVector4::OneVector;

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
			// BoneIndices
			FileStream.write(reinterpret_cast<char*>(&BoneIndices), sizeof(BoneIndices));
			// BoneWeights
			FileStream.write(reinterpret_cast<char*>(&BoneWeights), sizeof(BoneWeights));
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
			// BoneIndices
			InFileStream.read(reinterpret_cast<char*>(&BoneIndices), sizeof(BoneIndices));
			// BoneWeights
			InFileStream.read(reinterpret_cast<char*>(&BoneWeights), sizeof(BoneWeights));
		}
	};

}

/**
 * @brief Basic Buffer 구조체
 * Shader Reflection으로 셰이더 컴파일 할 때 이 버퍼들을 다 불러올 수 있다.
 * 그러니까 따로 이렇게 구조체를 만들지 않아도 무방하나,
 * 우리는 셰이더를 머티리얼을 통해서 접근해야하는데 그렇게 되면
 * 머티리얼과 다른 개체관의 관계의 결합이 생기기 때문에 따로 구조체를 만들어 뒀다.
 */
namespace Buffer
{
	// Landscape 버퍼 인스턴스
	struct FBufferInstance_LandScape
	{
		JArray<ComPtr<ID3D11Buffer>> Buffer_Vertex;			// Vertex 
		JArray<ComPtr<ID3D11Buffer>> Buffer_Index;			// Index
		JArray<ComPtr<ID3D11Buffer>> CBuffer_Space;			// Space

		void Resize(int32_t Size)
		{
			Buffer_Vertex.clear();
			Buffer_Index.clear();
			CBuffer_Space.clear();

			Buffer_Vertex.resize(Size);
			Buffer_Index.resize(Size);
			CBuffer_Space.resize(Size);
		}
	};

	// Basic 버퍼 인스턴스
	struct FBufferInstance
	{
		JArray<ComPtr<ID3D11Buffer>> Buffer_Vertex;			// Vertex 
		JArray<ComPtr<ID3D11Buffer>> Buffer_Index;			// Index

		void Resize(const int32_t Size)
		{
			Buffer_Vertex.clear();
			Buffer_Index.clear();

			Buffer_Vertex.resize(Size);
			Buffer_Index.resize(Size);
		}
	};

	// Bone 버퍼 인스턴스
	struct FBufferInstance_Bone
	{
		JArray<ComPtr<ID3D11Buffer>>             Buffer_Bone;	// Bone
		JArray<ComPtr<ID3D11ShaderResourceView>> Resource_Bone;	// Bone

		void Resize(const int32_t Size)
		{
			Buffer_Bone.clear();
			Resource_Bone.clear();

			Buffer_Bone.resize(Size);
			Resource_Bone.resize(Size);
		}
	};
}

// 인스턴싱 버퍼 구조체
struct FInstanceData
{
	FMatrix WorldMatrix;	// 각 인스턴스 별로 다른 월드 행렬

	// Etc... 머티리얼
};

struct FRenderCommand
{
	void*         ObjectToRender;	// 렌더 대상 오브젝트
	FInstanceData InstanceData;		// 인스턴싱 데이터
};

using RenderCommandList = JArray<FRenderCommand>;

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

	int32_t              FaceCount = 0;
	JArray<T>            VertexArray;
	JArray<uint32_t>     IndexArray;
	JArray<FTriangle<T>> TriangleList;

	uint32_t GetType() const override
	{
		return StringHash("VertexData");
	}

	static uint32_t GetVertexSize()
	{
		return sizeof(T);
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

#pragma once
#include <d3d11.h>

#include "Core/Graphics/ShaderStructs.h"

const uint32_t HASH_SHADER_SKELETAL_MESH     = StringHash(L"Character");
const uint32_t HASH_SHADER_STATIC_MESH       = StringHash(L"Basic");
const uint32_t HASH_SHADER_WorldGridMaterial = StringHash(L"WorldGridMaterial");
const uint32_t HASH_SHADER_SIMPLE_MESH       = StringHash(L"SimpleShape");
const uint32_t HASH_SHADER_TOON              = StringHash(L"Toon");
const uint32_t HASH_SHADER_GNOMON            = StringHash(L"Gnomon");
const uint32_t HASH_UVANIM                   = StringHash(L"UVAnim");

const uint32_t HASH_SHADER_SFXAA = StringHash(L"Shader/FXAA.hlsl");


namespace InputLayout
{
	constexpr D3D11_INPUT_ELEMENT_DESC Position =
	{
		"POSITION",                  // 셰이더 입력 서명에서 이 요소와 연결된 의미체계 (그러니깐 Semantic)
		0,                           // 의미상 인덱스
		DXGI_FORMAT_R32G32B32_FLOAT, // 데이터 형식 (float2)
		0,                           // 입력 어셈블러 식별정수
		0,                           // 요소 사이 오프셋 
		D3D11_INPUT_PER_VERTEX_DATA, // 단일 입력 슬롯 입력 데이터 클래스
		0                            // 정점 버퍼에서 렌더링 되는 인스턴스의 수 (D3D11_INPUT_PER_VERTEX_DATA -> 0)
	};

	constexpr D3D11_INPUT_ELEMENT_DESC Normal =
	{
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	};

	constexpr D3D11_INPUT_ELEMENT_DESC Color =
	{
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	};

	constexpr D3D11_INPUT_ELEMENT_DESC Texture =
	{
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	};

	constexpr D3D11_INPUT_ELEMENT_DESC Index =
	{
		"INDEX",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	};

	constexpr D3D11_INPUT_ELEMENT_DESC Weight =
	{
		"WEIGHT",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	};

	constexpr D3D11_INPUT_ELEMENT_DESC Tangent =
	{
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	};

	constexpr D3D11_INPUT_ELEMENT_DESC Binormal =
	{
		"BINORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	};

	constexpr D3D11_INPUT_ELEMENT_DESC FBX_MODEL_LAYOUT[] =
	{
		Position,
		Normal,
		Color,
		Texture,
		Index,
		Weight,
		Tangent
	};


	constexpr D3D11_INPUT_ELEMENT_DESC LAYOUT_StaticMesh_Base[] =
	{
		Position,
		Texture,
		Color,
		Normal,
		Tangent,
		Binormal
	};

	constexpr D3D11_INPUT_ELEMENT_DESC SIMPLE_SHAPE_LAYOUT[] =
	{
		Position,
		Texture
	};

	constexpr D3D11_INPUT_ELEMENT_DESC GNOMON_LAYOUT[] =
	{
		Position
	};

	inline JHash<uint32_t, const D3D11_INPUT_ELEMENT_DESC*> HASH_INPUT_LAYOUT_MAP_DESC =
	{
		{HASH_SHADER_STATIC_MESH, LAYOUT_StaticMesh_Base},
		{HASH_SHADER_WorldGridMaterial, LAYOUT_StaticMesh_Base},
		{HASH_SHADER_TOON, LAYOUT_StaticMesh_Base},
		{HASH_UVANIM, LAYOUT_StaticMesh_Base},
		{HASH_SHADER_SFXAA, SIMPLE_SHAPE_LAYOUT},
		{HASH_SHADER_GNOMON, GNOMON_LAYOUT}
	};

	inline JHash<uint32_t, uint32_t> HASH_INPUT_LAYOUT_MAP_NUMELEMENT =
	{
		{HASH_SHADER_STATIC_MESH, 6},
		{HASH_SHADER_WorldGridMaterial, 6},
		{HASH_SHADER_TOON, 6},
		{HASH_UVANIM, 6},
		{HASH_SHADER_SFXAA, 2},
		{HASH_SHADER_GNOMON, 1}
	};

	inline JHash<uint32_t, int32_t> HASH_VERTEX_SIZE_MAP =
	{
		{HASH_SHADER_STATIC_MESH, sizeof(Vertex::FVertexInfo_Base)},
		{HASH_SHADER_WorldGridMaterial, sizeof(Vertex::FVertexInfo_Base)},
		{HASH_SHADER_TOON, sizeof(Vertex::FVertexInfo_Base)},
		{HASH_UVANIM, sizeof(Vertex::FVertexInfo_Base)},
		{HASH_SHADER_SFXAA, sizeof(Vertex::FVertexInfo_Base)},
		{HASH_SHADER_GNOMON, sizeof(Vertex::FVertexInfo_Base)}
	};

}

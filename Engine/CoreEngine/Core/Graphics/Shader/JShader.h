#pragma once
#include "JConstantBuffer.h"
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Manager/IManagedInterface.h"

class JSceneComponent;
constexpr const char* NAME_SHADER_BASIC  = "Shader/Basic.hlsl";
constexpr const char* NAME_SHADER_GNOMON = "Shader/Gnomon.hlsl";

const uint32_t HASH_SHADER_BASIC  = StringHash(NAME_SHADER_BASIC);
const uint32_t HASH_SHADER_GNOMON = StringHash(NAME_SHADER_GNOMON);

struct JConstantBuffer;

struct FShaderData
{
	ComPtr<ID3D11InputLayout>    InputLayout;
	ComPtr<ID3D11VertexShader>   VertexShader;
	ComPtr<ID3D11PixelShader>    PixelShader;
	ComPtr<ID3D11GeometryShader> GeometryShader;
	ComPtr<ID3D11HullShader>     HullShader;
	ComPtr<ID3D11DomainShader>   DomainShader;
	ComPtr<ID3D11ComputeShader>  ComputeShader;

	ComPtr<ID3DBlob> VertexShaderBuf;
	ComPtr<ID3DBlob> PixelShaderBuf;
	ComPtr<ID3DBlob> GeometryShaderBuf;
	ComPtr<ID3DBlob> HullShaderBuf;
	ComPtr<ID3DBlob> DomainShaderBuf;
	ComPtr<ID3DBlob> ComputeShaderBuf;

	JArray<JConstantBuffer> ConstantBuffers;
	JHash<JText, int32_t>   ConstantBufferHashTable;
};

class JTexture;
/**
 * Shader파일 단위 관리
 * IManager->ShaderManager
 *
 * Shader, Blob Buffer(Vertex, Pixel), InputLayout에 대해서는 기본으로 가지고있다.
 * 파일내부의 실행함수를 변경하여 적용하려면 Load...Shader(FuncName)을 실행
 *
 * FIXME : 셰이더와 머티리얼을 분리해야할까? 
 */
class JShader
{

public:
	JShader(const JText& InName, LPCSTR VSEntryPoint = "VS", LPCSTR PSEntryPoint = "PS");
	JShader(const JWText& InName, LPCSTR VSEntryPoint = "VS", LPCSTR PSEntryPoint = "PS");
	virtual ~JShader();

public:
	void BindShaderPipeline(ID3D11DeviceContext* InDeviceContext);

	JArray<JConstantBuffer> GetConstantData() { return mShaderData.ConstantBuffers; }

	template <typename BufferData>
	void UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName, const BufferData& InData)
	{
		if (!mShaderData.ConstantBufferHashTable.contains(InBufferName))
		{
			LOG_CORE_ERROR("Buffer Name is not found in Shader Constant Buffer Table");
			return;
		}

		const int32_t index = mShaderData.ConstantBufferHashTable[InBufferName];

		Utils::DX::UpdateDynamicBuffer(
									   InDeviceContext,
									   mShaderData.ConstantBuffers[index].Buffer.Get(),
									   &InData,
									   sizeof(BufferData));
	}

	virtual void UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext);

private:
	void LoadVertexShader(LPCSTR FuncName);
	void LoadPixelShader(LPCSTR FuncName);
	void LoadGeometryShader(LPCSTR FuncName);
	void LoadHullShader(LPCSTR FuncName);
	void LoadDomainShader(LPCSTR FuncName);
	void LoadComputeShader(LPCSTR FuncName);

private:
	void LoadShaderReflectionData();

public:
	[[nodiscard]] FORCEINLINE const JWText& GetShaderFile() const { return mShaderFile; }

protected:
	JWText mShaderFile;

	
	FShaderData mShaderData;
};

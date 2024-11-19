#pragma once
#include "JConstantBuffer.h"
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Graphics/ShaderStructs.h"

constexpr const char* NAME_SHADER_BASIC  = "Shader/Basic.hlsl";
constexpr const char* NAME_SHADER_GNOMON = "Shader/Gnomon.hlsl";
constexpr const char* NAME_SHADER_ID     = "Shader/FBMP.hlsl";

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

	uint32_t VertexInputLayoutSize;

	JArray<JConstantBuffer>  ConstantBuffers;
	JHash<uint32_t, int32_t> ConstantBufferHashTable;
};

class JTexture;


/**
 * Shader파일 단위 관리
 * GetWorld->ShaderManager
 *
 * Shader, Blob Buffer(Vertex, Pixel), InputLayout에 대해서는 기본으로 가지고있다.
 * 파일내부의 실행함수를 변경하여 적용하려면 Load...Shader(FuncName)을 실행
 *
 * FIXME : 셰이더와 머티리얼을 분리해야할까?
 * 이런 고민이 되는 이유는 머티리얼은 셰이더에 따라 다르게 적용되어야하는데
 * 셰이더에서는 머티리얼과 상관이 없는 정점 위치라던가 카메라 위치등이포함
 * 셰이더가 오브젝트에 붙을 수도있고 머티리얼에 붙을 수음
 * 우선 머티리얼에서 셰이더를 참조하도록 구현
 */
class JShader
{

public:
	JShader(const JText& InName, LPCSTR VSEntryPoint = "VS", LPCSTR PSEntryPoint = "PS");
	JShader(const JWText& InName, LPCSTR VSEntryPoint = "VS", LPCSTR PSEntryPoint = "PS");
	virtual ~JShader();

public:
	/**
	 * 셰이더 설정 및 상수 버퍼 설정
	 */
	void BindShaderPipeline(ID3D11DeviceContext* InDeviceContext);

public:
	/**
	 * 전체 상수버퍼 업데이트
	 */
	void UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName, const void* InData,
							const uint32_t       InOffset = 0);

	/**
	 * 상수버퍼 내부 데이터(특정 데이터) 업데이트
	 */
	void UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName, const JText& InDataName,
							const void*          InData);

	template <typename BufferData>
	void UpdateConstantDataTemplate(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName,
									const BufferData&    InData)
	{
		if (JConstantBuffer* buffer = GetConstantBuffer(InBufferName))
		{
			memcpy_s(buffer->Data.data(),
					 buffer->Data.size(),
					 &InData,
					 sizeof(BufferData));

			buffer->UpdateBuffer(InDeviceContext);
		}
	}

	/**
	 * 상수버퍼를 셰이더에 연결
	 */
	virtual void UpdateGlobalConstantBuffer(ID3D11DeviceContext* InDeviceContext);

public:
	JConstantBuffer* GetConstantBuffer(const JText& InBufferName);

private:
	void LoadVertexShader(LPCSTR FuncName);
	void LoadPixelShader(LPCSTR FuncName);
	void LoadGeometryShader(LPCSTR FuncName);
	void LoadHullShader(LPCSTR FuncName);
	void LoadDomainShader(LPCSTR FuncName);
	void LoadComputeShader(LPCSTR FuncName);
	void LoadShaderReflectionData();

public:
	[[nodiscard]] FORCEINLINE const JWText& GetShaderFile() const { return mShaderFile; }
	[[nodiscard]] FORCEINLINE uint32_t      GetInputLayoutSize() const { return mDefaultShaderData.VertexInputLayoutSize; }
	void                                    SetTargetCamera(class JCamera* InCamera) { mTargetCamera = InCamera; }

protected:
	JWText                    mShaderFile;
	FShaderData               mDefaultShaderData;
	JHash<JText, FShaderData> mSubShaderData;

	JCamera* mTargetCamera;		// 오브젝트가 참조하는 카메라 개체

	CBuffer::Space  mCachedSpaceData;
	CBuffer::Camera mCachedCameraData;
};

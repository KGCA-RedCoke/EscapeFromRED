#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Manager/IManagedInterface.h"

struct FShaderData
{
	ComPtr<ID3D11InputLayout>    InputLayout;
	ComPtr<ID3D11VertexShader>   VertexShader;
	ComPtr<ID3D11PixelShader>    PixelShader;
	ComPtr<ID3D11GeometryShader> GeometryShader;
	ComPtr<ID3D11HullShader>     HullShader;
	ComPtr<ID3D11DomainShader>   DomainShader;
	ComPtr<ID3D11ComputeShader>  ComputeShader;
	ComPtr<ID3DBlob>             VertexShaderBuf;
	ComPtr<ID3DBlob>             PixelShaderBuf;
	ComPtr<ID3DBlob>             GeometryShaderBuf;
	ComPtr<ID3DBlob>             HullShaderBuf;
	ComPtr<ID3DBlob>             DomainShaderBuf;
	ComPtr<ID3DBlob>             ComputeShaderBuf;
};

class JTexture;
/**
 * Shader파일 단위 관리
 * IManager->ShaderManager
 *
 * Shader, Blob Buffer(Vertex, Pixel), InputLayout에 대해서는 기본으로 가지고있다.
 * 파일내부의 실행함수를 변경하여 적용하려면 Load...Shader(FuncName)을 실행
 */
class JShader : public IManagedInterface
{

public:
	JShader(const JText& InShaderFile, LPCSTR VSEntryPoint = "VS", LPCSTR PSEntryPoint = "PS");
	JShader(const JWText& InShaderFile, LPCSTR VSEntryPoint = "VS", LPCSTR PSEntryPoint = "PS");
	~JShader() override;

public:
	uint32_t GetHash() const override;

public:
	void ApplyShader(ID3D11DeviceContext* InDeviceContext) const;

public:
	void LoadVertexShader(LPCSTR FuncName);
	void LoadPixelShader(LPCSTR FuncName);
	void LoadGeometryShader(LPCSTR FuncName);
	void LoadHullShader(LPCSTR FuncName);
	void LoadDomainShader(LPCSTR FuncName);
	void LoadComputeShader(LPCSTR FuncName);

private:
	void CreateInputLayout();

public:
	[[nodiscard]] FORCEINLINE const JWText& GetShaderFile() const { return mShaderFile; }

	// [[nodiscard]] FORCEINLINE FShaderData*       GetShaderData() { return &mShaderData; }
	// [[nodiscard]] FORCEINLINE const FShaderData* GetShaderData() const { return &mShaderData; }

protected:
	JWText mShaderFile;

	FShaderData mShaderData;

};

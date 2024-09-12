#pragma once
#include "Core/Graphics/graphics_common_include.h"

class JTexture;
/**
 * Shader파일 단위 관리
 * IManager->ShaderManager
 *
 * Shader, Blob Buffer(Vertex, Pixel), InputLayout에 대해서는 기본으로 가지고있다.
 * 파일내부의 실행함수를 변경하여 적용하려면 Load...Shader(FuncName)을 실행
 */
class JShader
{
public:
	explicit JShader(const JText& InShaderFile, LPCSTR VSEntryPoint = "VS", LPCSTR PSEntryPoint = "PS");
	explicit JShader(const JWText& InShaderFile, LPCSTR VSEntryPoint = "VS", LPCSTR PSEntryPoint = "PS");
	~JShader() = default;

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
	[[nodiscard]] FORCEINLINE ID3D11InputLayout*    GetInputLayout() const { return mInputLayout.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11VertexShader*   GetVertexShader() const { return mVertexShader.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11PixelShader*    GetPixelShader() const { return mPixelShader.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11GeometryShader* GetGeometryShader() const { return mGeometryShader.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11HullShader*     GetHullShader() const { return mHullShader.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11DomainShader*   GetDomainShader() const { return mDomainShader.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11ComputeShader*  GetComputeShader() const { return mComputeShader.Get(); }
	[[nodiscard]] FORCEINLINE ID3DBlob*             GetVertexShaderBuf() const { return mVertexShaderBuf.Get(); }
	[[nodiscard]] FORCEINLINE ID3DBlob*             GetPixelShaderBuf() const { return mPixelShaderBuf.Get(); }
	[[nodiscard]] FORCEINLINE ID3DBlob*             GetGeometryShaderBuf() const { return mGeometryShaderBuf.Get(); }
	[[nodiscard]] FORCEINLINE ID3DBlob*             GetHullShaderBuf() const { return mHullShaderBuf.Get(); }
	[[nodiscard]] FORCEINLINE ID3DBlob*             GetDomainShaderBuf() const { return mDomainShaderBuf.Get(); }
	[[nodiscard]] FORCEINLINE ID3DBlob*             GetComputeShaderBuf() const { return mComputeShaderBuf.Get(); }
	[[nodiscard]] JTexture*                         GetDefaultAlbedoTexture() const { return mDefaultAlbedoTexture; }
	[[nodiscard]] JTexture*                         GetDefaultNormalTexture() const { return mDefaultNormalTexture; }

private:
	JWText mShaderFile;

	ComPtr<ID3D11InputLayout> mInputLayout;

public:

private:
	ComPtr<ID3D11VertexShader>   mVertexShader;
	ComPtr<ID3D11PixelShader>    mPixelShader;
	ComPtr<ID3D11GeometryShader> mGeometryShader;
	ComPtr<ID3D11HullShader>     mHullShader;
	ComPtr<ID3D11DomainShader>   mDomainShader;
	ComPtr<ID3D11ComputeShader>  mComputeShader;

	ComPtr<ID3DBlob> mVertexShaderBuf;
	ComPtr<ID3DBlob> mPixelShaderBuf;
	ComPtr<ID3DBlob> mGeometryShaderBuf;
	ComPtr<ID3DBlob> mHullShaderBuf;
	ComPtr<ID3DBlob> mDomainShaderBuf;
	ComPtr<ID3DBlob> mComputeShaderBuf;

	JTexture* mDefaultAlbedoTexture;
	JTexture* mDefaultNormalTexture;
};

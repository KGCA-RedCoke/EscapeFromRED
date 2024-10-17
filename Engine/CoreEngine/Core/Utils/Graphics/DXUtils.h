#pragma once
#include "common_include.h"
#include "Core/Graphics/graphics_common_include.h"


namespace Utils::DX
{
#pragma region ----------------------- Device Creation -----------------------
	void CreateRenderTarget(ID3D11Device*                    InDevice,
							uint32_t                         InWidth,
							uint32_t                         InHeight,
							_Out_ ID3D11RenderTargetView**   OutRTV,
							_Out_ ID3D11ShaderResourceView** OutSRV,
							_Out_ ID3D11Texture2D**          OutTexture);

	void CreateD2DRenderTarget(ID2D1Factory*             InFactory,
							   _In_ ID3D11Texture2D*     InTexture,
							   _Out_ ID2D1RenderTarget** OutRenderTarget);

	void CreateViewport(uint32_t              InWidth,
						uint32_t              InHeight,
						_Out_ D3D11_VIEWPORT* OutViewport);

	void CreateDepthStencilView(ID3D11Device*                  InDevice,
								uint32_t                       InWidth,
								uint32_t                       InHeight,
								_Out_ ID3D11DepthStencilView** OutDSV,
								_Out_ ID3D11Texture2D**        OutTexture);

	void CreateDepthStencilState(ID3D11Device*                   InDevice,
								 bool                            InDepthEnable,
								 D3D11_DEPTH_WRITE_MASK          InDepthWriteMask,
								 D3D11_COMPARISON_FUNC           InDepthFunc,
								 _Out_ ID3D11DepthStencilState** OutState);
#pragma endregion

	HRESULT LoadVertexShader(ID3D11Device*        Device, const JWText& VertexFileName,
							 ID3D11VertexShader** VertexShader,
							 ID3DBlob**           OutBlob = nullptr, LPCSTR EntryPoint = nullptr);
	HRESULT LoadPixelShader(ID3D11Device*       Device, const JWText& PixelFileName,
							ID3D11PixelShader** pixelShader,
							ID3DBlob**          OutBlob = nullptr, LPCSTR EntryPoint = nullptr);

	HRESULT LoadGeometryShader(ID3D11Device*          Device, const JWText&      ShaderFile,
							   ID3D11GeometryShader** geometryShader, ID3DBlob** OutBlob = nullptr,
							   LPCSTR                 FuncName                           = nullptr);
	HRESULT LoadHullShaderFile(ID3D11Device*      Device, const JWText&,
							   ID3D11HullShader** hullShader, ID3DBlob** OutBlob = nullptr,
							   LPCSTR             FuncName                       = nullptr);
	HRESULT LoadDomainShaderFile(ID3D11Device*        Device, const JWText&    ShaderFile,
								 ID3D11DomainShader** domainShader, ID3DBlob** OutBlob = nullptr,
								 LPCSTR               FuncName                         = nullptr);
	HRESULT LoadComputeShaderFile(ID3D11Device*         Device, const JWText& ShaderFile,
								  ID3D11ComputeShader** computeShader,
								  ID3DBlob**            OutBlob  = nullptr,
								  LPCSTR                FuncName = nullptr);


	HRESULT CompileShader(const WCHAR* FileName, LPCSTR        EntryPoint,
						  LPCSTR       ShaderModel, ID3DBlob** OutBlob);

	/**
	 * @brief ID3D11Buffer 버퍼 생성을 시도 실패시 종료
	 * @param InDevice
	 * @param InBindFlag Vertex, Index, Constant 지정
	 * @param InData 버퍼에 넣을 데이터
	 * @param InDataSize 데이터 블록 단위
	 * @param InDataNum 데이터 배열 갯수
	 * @param OutBuffer 반환 버퍼
	 * @param InUsage 런타임에 자주 업데이트 된다면 D3D11_USAGE_DYNAMIC
	 * @param InAccessFlag Map을 사용할거면 D3D11_CPU_ACCESS_WRITE로 지정
	 */
	void CreateBuffer(
		ID3D11Device*        InDevice,
		D3D11_BIND_FLAG      InBindFlag,
		void**               InData,
		uint32_t             InDataSize,
		uint32_t             InDataNum,
		_Out_ ID3D11Buffer** OutBuffer,
		D3D11_USAGE          InUsage      = D3D11_USAGE_DEFAULT,
		UINT                 InAccessFlag = 0
	);

	/**
	 * @brief 버퍼내용 일괄 업데이트 (Usage가 Dynamic이 아니여도 상관없다)
	 * @param InDeviceContext
	 * @param InBuffer 업데이트 할 버퍼
	 * @param InData 새 데이터
	 */
	void UpdateBuffer(ID3D11DeviceContext* InDeviceContext,
					  ID3D11Buffer*        InBuffer,
					  const void*          InData);

	/**
	 * @brief 버퍼내용 업데이트 (버퍼의 Usage가 D3D11_USAGE_DYNAMIC이여야 함 (Default X))
	 * 약간의 성능상 이점이 있을수도..
	 * @param InDeviceContext
	 * @param InBuffer
	 * @param InData
	 * @param InSize
	 */
	void UpdateDynamicBuffer(ID3D11DeviceContext* InDeviceContext,
							 ID3D11Buffer*        InBuffer,
							 const void*          InData,
							 UINT                 InSize);


	FVector ComputeFaceNormal(int32_t Index0, int32_t Index1, int32_t Index2, const float* InHeightMap);
}

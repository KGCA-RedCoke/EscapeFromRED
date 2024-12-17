#include "DXUtils.h"

#include <d3dcompiler.h>

#include "Core/Graphics/Shader/InputLayouts.h"
#include "Core/Graphics/Texture/JTexture.h"

namespace Utils::DX
{

#pragma region ----------------------- Device Creation -----------------------
	void CreateRenderTarget(ID3D11Device* InDevice, uint32_t InWidth, uint32_t InHeight, ID3D11RenderTargetView** OutRTV,
							ID3D11ShaderResourceView** OutSRV, ID3D11Texture2D** OutTexture)
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width            = InWidth;
		desc.Height           = InHeight;
		desc.MipLevels        = 1;
		desc.ArraySize        = 1;
		desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage            = D3D11_USAGE_DEFAULT;
		desc.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		CheckResult(InDevice->CreateTexture2D(&desc, nullptr, OutTexture));
		CheckResult(InDevice->CreateRenderTargetView(*OutTexture, nullptr, OutRTV));
		CheckResult(InDevice->CreateShaderResourceView(*OutTexture, nullptr, OutSRV));
	}

	void CreateD2DRenderTarget(ID2D1Factory* InFactory, ID3D11Texture2D* InTexture, ID2D1RenderTarget** OutRenderTarget)
	{
		ComPtr<IDXGISurface> dxgiBackBuffer;

		D2D1_RENDER_TARGET_PROPERTIES props;
		{
			props.type        = D2D1_RENDER_TARGET_TYPE_DEFAULT;
			props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
			props.dpiX        = 96.f;
			props.dpiY        = 96.f;
			props.usage       = D2D1_RENDER_TARGET_USAGE_NONE;
			props.minLevel    = D2D1_FEATURE_LEVEL_DEFAULT;
		}


		CheckResult(
					InTexture->QueryInterface( // Query Interface
											  __uuidof(IDXGISurface), // 내가 원하는 자료형(ID)
											  reinterpret_cast<void**>(dxgiBackBuffer.GetAddressOf() // 담아질 개체
											  )));
		CheckResult(InFactory->
					CreateDxgiSurfaceRenderTarget(
												  dxgiBackBuffer.Get(),
												  &props,
												  OutRenderTarget));

		dxgiBackBuffer = nullptr;
	}

	void CreateViewport(uint32_t InWidth, uint32_t InHeight, D3D11_VIEWPORT* OutViewport, FVector2 InOffset)
	{
		OutViewport->Width    = static_cast<FLOAT>(InWidth);
		OutViewport->Height   = static_cast<FLOAT>(InHeight);
		OutViewport->MinDepth = 0.0f;
		OutViewport->MaxDepth = 1.0f;
		OutViewport->TopLeftX = InOffset.x;
		OutViewport->TopLeftY = InOffset.y;
	}

	void CreateDepthStencilView(ID3D11Device*            InDevice, uint32_t        InWidth, uint32_t InHeight,
								ID3D11DepthStencilView** OutDSV, ID3D11Texture2D** OutTexture)
	{
		// ------------------------- DepthBuffer -------------------------
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width              = InWidth;
		depthStencilDesc.Height             = InHeight;
		depthStencilDesc.MipLevels          = 1;
		depthStencilDesc.ArraySize          = 1;
		depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count   = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags     = 0; // CPU 접근 가능
		depthStencilDesc.MiscFlags          = 0;
		CheckResult(InDevice->CreateTexture2D(&depthStencilDesc, nullptr, OutTexture));

		// ------------------------- Depth Stencil View -------------------------
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		CheckResult(InDevice->CreateDepthStencilView(*OutTexture,
													 &depthStencilViewDesc,
													 OutDSV));

	}

	void CreateDepthStencilState(ID3D11Device* InDevice, bool InDepthEnable, D3D11_DEPTH_WRITE_MASK InDepthWriteMask,
								 D3D11_COMPARISON_FUNC InDepthFunc, ID3D11DepthStencilState** OutState)
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDescState{};
		depthStencilDescState.DepthEnable    = InDepthEnable;
		depthStencilDescState.DepthWriteMask = InDepthWriteMask;
		depthStencilDescState.DepthFunc      = InDepthFunc;

		// Stencil 설정은 나중에 추가
		// depthStencilDescState.StencilEnable  = false;
		// depthStencilDescState.StencilReadMask = 0xFF;
		// depthStencilDescState.StencilWriteMask = 0xFF;
		// depthStencilDescState.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		// depthStencilDescState.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		// depthStencilDescState.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		// depthStencilDescState.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		// depthStencilDescState.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

		CheckResult(InDevice->CreateDepthStencilState(&depthStencilDescState, OutState));
	}
#pragma endregion


	HRESULT LoadVertexShader(ID3D11Device* Device, const JWText& VertexFileName, ID3D11VertexShader** VertexShader,
							 ID3DBlob**    OutBlob, LPCSTR       EntryPoint)
	{
		ID3DBlob* blob;
		HRESULT   result = CompileShader(VertexFileName.c_str(),
									   EntryPoint,
									   "vs_5_0",
									   &blob);
		if (FAILED(result))
		{
			return result;
		}

		DWORD   size = blob->GetBufferSize();
		LPCVOID data = blob->GetBufferPointer();

		if (FAILED(result = Device->CreateVertexShader(data, size, nullptr, VertexShader)))
		{
			blob = nullptr;
			return result;
		}

		if (!OutBlob)
		{
			blob = nullptr;
		}
		else
		{
			*OutBlob = blob;
		}
		return result;
	}

	HRESULT LoadPixelShader(ID3D11Device* Device, const JWText& PixelFileName, ID3D11PixelShader** pixelShader,
							ID3DBlob**    OutBlob, LPCSTR       EntryPoint)
	{
		ID3DBlob* blob;
		HRESULT   result = CompileShader(PixelFileName.c_str(),
									   EntryPoint,
									   "ps_5_0",
									   &blob);
		if (FAILED(result))
		{
			return result;
		}

		DWORD   size = blob->GetBufferSize();
		LPCVOID data = blob->GetBufferPointer();

		if (FAILED(result = Device->CreatePixelShader(data, size, nullptr, pixelShader)))
		{
			blob = nullptr;
			return result;
		}

		if (!OutBlob)
		{
			blob = nullptr;
		}
		else
		{
			*OutBlob = blob;
		}
		return result;
	}

	HRESULT LoadGeometryShader(ID3D11Device* Device, const JWText& ShaderFile, ID3D11GeometryShader** geometryShader,
							   ID3DBlob**    OutBlob, LPCSTR       FuncName)
	{
		ID3DBlob* blob;

		HRESULT result = CompileShader(ShaderFile.c_str(),
									   FuncName ? FuncName : "GS",
									   "gs_5_0",
									   &blob);

		size_t  size = blob->GetBufferSize();
		LPCVOID data = blob->GetBufferPointer();

		if (FAILED(Device->CreateGeometryShader(data, size, nullptr, geometryShader)))
		{
			blob = nullptr;
			return result;
		}

		if (!OutBlob)
		{
			blob->Release();
		}
		else
		{
			*OutBlob = blob;
		}

		return result;
	}

	HRESULT LoadHullShaderFile(ID3D11Device* Device, const JWText& ShaderFile, ID3D11HullShader** hullShader,
							   ID3DBlob**    OutBlob, LPCSTR       FuncName)
	{
		ID3DBlob* blob;

		HRESULT result = CompileShader(ShaderFile.c_str(),
									   FuncName ? FuncName : "HS",
									   "hs_5_0",
									   &blob);

		size_t  size = blob->GetBufferSize();
		LPCVOID data = blob->GetBufferPointer();

		if (FAILED(Device->CreateHullShader(data, size, nullptr, hullShader)))
		{
			blob = nullptr;
			return result;
		}

		if (!OutBlob)
		{
			blob->Release();
		}
		else
		{
			*OutBlob = blob;
		}

		return result;
	}

	HRESULT LoadDomainShaderFile(ID3D11Device* Device, const JWText& ShaderFile, ID3D11DomainShader** domainShader,
								 ID3DBlob**    OutBlob, LPCSTR       FuncName)
	{
		ID3DBlob* blob;

		HRESULT result = CompileShader(ShaderFile.c_str(),
									   FuncName ? FuncName : "DS",
									   "ds_5_0",
									   &blob);

		size_t  size = blob->GetBufferSize();
		LPCVOID data = blob->GetBufferPointer();

		if (FAILED(Device->CreateDomainShader(data, size, nullptr, domainShader)))
		{
			blob = nullptr;
			return result;
		}

		if (!OutBlob)
		{
			blob->Release();
		}
		else
		{
			*OutBlob = blob;
		}

		return result;
	}

	HRESULT LoadComputeShaderFile(ID3D11Device* Device, const JWText& ShaderFile, ID3D11ComputeShader** computeShader,
								  ID3DBlob**    OutBlob, LPCSTR       FuncName)
	{
		ID3DBlob* blob;

		HRESULT result = CompileShader(ShaderFile.c_str(),
									   FuncName ? FuncName : "CS",
									   "cs_5_0",
									   &blob);

		size_t  size = blob->GetBufferSize();
		LPCVOID data = blob->GetBufferPointer();

		if (FAILED(Device->CreateComputeShader(data, size, nullptr, computeShader)))
		{
			blob = nullptr;
			return result;
		}

		if (!OutBlob)
		{
			blob->Release();
		}
		else
		{
			*OutBlob = blob;
		}

		return result;
	}


	HRESULT CompileShader(const WCHAR* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** OutBlob)
	{
		HRESULT result;

		DWORD ShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		ShaderFlag |= D3DCOMPILE_DEBUG;
#endif
		ComPtr<ID3DBlob> errorBlob;

		result = D3DCompileFromFile(
									FileName,    // 셰이더 파일명
									nullptr,     // 코드 안 매크로 배열 주소
									D3D_COMPILE_STANDARD_FILE_INCLUDE,     // ID3DInclude 인터페이스
									EntryPoint,  // main 진입점
									ShaderModel, // 셰이더 프로필
									ShaderFlag,  // 셰이더 컴파일 플래그
									0,           // 이펙트 옵션 컴파일 플래그
									OutBlob,     // 반환 될 blob
									errorBlob.GetAddressOf()// 컴파일 오류 및 경고 목록 저장
								   );

		if (FAILED(result))
		{
			if (errorBlob.Get())
			{
				LOG_CORE_ERROR("Failed to compile shader: {0}", static_cast<char*>(errorBlob->GetBufferPointer()));
				OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
			}
		}

		if (errorBlob.Get())
		{
			errorBlob = nullptr;
		}

		return result;
	}

	ID3D11ShaderResourceView* CreateTextureArray(
		ID3D11Device*            device,
		ID3D11DeviceContext*     context,
		const JArray<JTexture*>& textures)
	{
		if (textures.empty())
			throw std::runtime_error("No textures provided to create the texture array.");

		// Get the description of the first texture to determine array format
		D3D11_TEXTURE2D_DESC textureDesc = textures[0]->GetTextureDesc();

		// Validate that all textures have the same format, dimensions, and mip levels
		for (size_t i = 1; i < textures.size(); ++i)
		{
			D3D11_TEXTURE2D_DESC otherDesc = textures[i]->GetTextureDesc();
			if (textureDesc.Width != otherDesc.Width ||
				textureDesc.Height != otherDesc.Height ||
				textureDesc.MipLevels != otherDesc.MipLevels ||
				textureDesc.Format != otherDesc.Format)
			{
				throw std::runtime_error("All textures must have the same dimensions, format, and mip levels.");
			}
		}

		// Adjust for Texture2DArray
		textureDesc.ArraySize      = static_cast<UINT>(textures.size());
		textureDesc.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.Usage          = D3D11_USAGE_DEFAULT;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags      = 0;

		// Create the texture array
		ComPtr<ID3D11Texture2D> textureArray;
		HRESULT                 hr = device->CreateTexture2D(&textureDesc, nullptr, &textureArray);
		if (FAILED(hr))
			throw std::runtime_error("Failed to create Texture2D array.");

		// Copy each individual texture into the array
		for (UINT i = 0; i < textures.size(); ++i)
		{
			for (UINT mip = 0; mip < textureDesc.MipLevels; ++mip)
			{
				D3D11_BOX sourceRegion = {
					0,
					0,
					0,
					textureDesc.Width >> mip,
					textureDesc.Height >> mip,
					1
				};

				context->CopySubresourceRegion(
											   textureArray.Get(),
											   D3D11CalcSubresource(mip, i, textureDesc.MipLevels),
											   0,
											   0,
											   0,
											   textures[i]->GetTexture2D(),
											   mip,
											   &sourceRegion
											  );
			}
		}

		// Create Shader Resource View
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format                          = textureDesc.Format;
		srvDesc.ViewDimension                   = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MostDetailedMip  = 0;
		srvDesc.Texture2DArray.MipLevels        = textureDesc.MipLevels;
		srvDesc.Texture2DArray.FirstArraySlice  = 0;
		srvDesc.Texture2DArray.ArraySize        = textureDesc.ArraySize;

		ComPtr<ID3D11ShaderResourceView> srv;
		hr = device->CreateShaderResourceView(textureArray.Get(), &srvDesc, &srv);
		if (FAILED(hr))
			throw std::runtime_error("Failed to create ShaderResourceView for Texture2D array.");

		return srv.Detach();
	}

	void CreateBuffer(ID3D11Device* InDevice, D3D11_BIND_FLAG InBindFlag, void**     InData, uint32_t InDataSize,
					  uint32_t      InDataNum, ID3D11Buffer** OutBuffer, D3D11_USAGE InUsage, UINT    InAccessFlag)
	{
		D3D11_BUFFER_DESC bufferDesc;
		{
			bufferDesc.ByteWidth      = InDataNum * InDataSize; // 버퍼크기
			bufferDesc.Usage          = InUsage;	// 버퍼의 읽기/쓰기 방법 지정
			bufferDesc.BindFlags      = InBindFlag; // 파이프라인에 바인딩될 방법
			bufferDesc.CPUAccessFlags = InAccessFlag; // 생성될 버퍼에 CPU가 접근하는 유형 (DX 성능에 매우 중요)
			bufferDesc.MiscFlags      = 0; // 추가적인 옵션 플래그
		}

		D3D11_SUBRESOURCE_DATA data;
		{
			data.pSysMem = InData; // 초기화 데이터 포인터 (정점 배열의 주소를 넘겨준다)
			// InitData.SysMemPitch (텍스처 리소스의 한줄의 크기)
			// InitData.SysMemSlicePitch (3차원 텍스처의 깊이 간격)
		}

		CheckResult(InDevice->CreateBuffer(
										   &bufferDesc,
										   InData ? &data : nullptr,
										   OutBuffer
										  ));
	}

	void UpdateBuffer(ID3D11DeviceContext* InDeviceContext, ID3D11Buffer* InBuffer, const void* InData)
	{
		InDeviceContext->UpdateSubresource(
										   InBuffer,                 // 업데이트할 버퍼
										   0,                      // Subresource index (0은 전체 버퍼를 의미)
										   nullptr,					// 데이터 업데이트 영역의 최상위 좌표 (전체 버퍼를 업데이트할 때는 nullptr)
										   InData,                   // 새 데이터
										   0,                      // 데이터의 행 간격 (옵션, 일반적으로 0)
										   0                       // 데이터의 슬라이스 간격 (옵션, 일반적으로 0)
										  );
	}

	void UpdateDynamicBuffer(ID3D11DeviceContext* InDeviceContext, ID3D11Buffer* InBuffer, const void* InData, UINT InSize)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (SUCCEEDED(InDeviceContext->Map(
						  InBuffer,					// 버퍼
						  0,						// Subresource index (0은 전체 버퍼를 의미)
						  D3D11_MAP_WRITE_DISCARD,	// 맵핑 플래그
						  0,						// 맵핑 플래그
						  &mappedResource			// 데이터에 대한 접근을 제공하는 구조체
					  )))
		{
			// 데이터를 복사
			memcpy(mappedResource.pData, InData, InSize);

			// 맵핑 해제
			InDeviceContext->Unmap(InBuffer, 0);
		}
	}

	FVector ComputeFaceNormal(int32_t Index0, int32_t Index1, int32_t Index2, const float* InHeightMap)
	{

		FVector v0 = InHeightMap[Index1] - InHeightMap[Index0];
		FVector v1 = InHeightMap[Index2] - InHeightMap[Index0];

		FVector normal = v0.Cross(v1);
		normal.Normalize();

		return normal;
	}

	float GetDepthValue(ID3D11Device*           InDevice, ID3D11DeviceContext* InDeviceContext,
						ID3D11DepthStencilView* InDepthStencilView,
						const FVector2&         InScreenPos, const FVector2& InScreenSize)
	{
		float depthValue = 0.0f;

		ComPtr<ID3D11Texture2D> depthStencilTexture = nullptr;
		ComPtr<ID3D11Texture2D> pStagingTexture     = nullptr;

		D3D11_TEXTURE2D_DESC stagingDesc;
		stagingDesc.Width              = InScreenPos.x;
		stagingDesc.Height             = InScreenPos.y;
		stagingDesc.MipLevels          = 1;
		stagingDesc.ArraySize          = 1;
		stagingDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
		stagingDesc.SampleDesc.Count   = 1;
		stagingDesc.SampleDesc.Quality = 0;
		stagingDesc.Usage              = D3D11_USAGE_STAGING;
		stagingDesc.BindFlags          = 0;
		stagingDesc.CPUAccessFlags     = D3D11_CPU_ACCESS_READ; // CPU 접근 가능
		stagingDesc.MiscFlags          = 0;

		CheckResult(InDevice->CreateTexture2D(&stagingDesc, nullptr, pStagingTexture.GetAddressOf()));

		InDepthStencilView->GetResource(reinterpret_cast<ID3D11Resource**>(depthStencilTexture.GetAddressOf()));
		InDeviceContext->CopyResource(pStagingTexture.Get(), depthStencilTexture.Get());

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(InDeviceContext->Map(pStagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource)))
		{
			uint32_t* data = static_cast<uint32_t*>(mappedResource.pData);

			const int32_t index = InScreenPos.y * mappedResource.RowPitch / sizeof(float) + InScreenPos.x;

			depthValue = static_cast<float>(data[index]);

			InDeviceContext->Unmap(pStagingTexture.Get(), 0);
		}

		return depthValue;
	}

	FVector2 Screen2DToNDC(const FVector2& InScreenPos, const FVector2& InScreenSize)
	{
		float ndcX = (2.0f * InScreenPos.x / InScreenSize.x) - 1.0f;
		float ndcY = 1.0f - (2.0f * InScreenPos.y / InScreenSize.y);

		return FVector2(ndcX, ndcY);
	}

	FVector Screen2World(const FVector2& InScreenPos, const FVector2&    InScreenSize, const FMatrix& InViewMatrix,
						 const FMatrix&  InProjectionMatrix, const float InDepth)
	{
		// 정규화된 디바이스 좌표로 변환
		FVector2 ndc = Screen2DToNDC(InScreenPos, InScreenSize);

		// NDC -> 카메라 공간
		DirectX::XMMATRIX invProjection  = DirectX::XMMatrixInverse(nullptr, InProjectionMatrix);
		DirectX::XMVECTOR mouseNDC       = DirectX::XMVectorSet(ndc.x, ndc.y, InDepth, 1.0f);
		DirectX::XMVECTOR cameraSpacePos = DirectX::XMVector3TransformCoord(mouseNDC, invProjection);

		// 카메라 공간 -> 월드 공간
		DirectX::XMMATRIX invView  = DirectX::XMMatrixInverse(nullptr, InViewMatrix);
		DirectX::XMVECTOR worldPos = DirectX::XMVector3TransformCoord(cameraSpacePos, invView);

		FVector returnValue;
		XMStoreFloat3(&returnValue, worldPos);
		XMFLOAT3 worldPosition;
		XMStoreFloat3(&worldPosition,
					  XMVector3Unproject(XMVectorSet(ndc.x, ndc.y, InDepth, 1.0f),
										 0,
										 0,
										 InScreenSize.x,
										 InScreenSize.y,
										 0.0f,
										 1.0f,
										 InProjectionMatrix,
										 InViewMatrix,
										 XMMatrixIdentity()));

		return FVector(worldPosition);
	}


}

#include "DXUtils.h"

#include <d3dcompiler.h>

#include "Core/Graphics/Shader/InputLayouts.h"

namespace Utils::DX
{
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
}

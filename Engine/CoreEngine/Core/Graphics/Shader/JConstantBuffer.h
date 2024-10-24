#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/Graphics/DXUtils.h"

struct FCBufferVariable
{
	JText    Name;
	uint32_t Offset;
	uint32_t Size;

	FCBufferVariable() = default;

	FCBufferVariable(const JText& InName, uint32_t InOffset, uint32_t InSize)
		: Name(InName),
		  Offset(InOffset),
		  Size(InSize)
	{}
};

struct JConstantBuffer
{
	JText                    Name;
	uint32_t                 Hash;
	uint32_t                 Size;
	uint32_t                 Slot;
	bool                     bPassToVertexShader;
	bool                     bPassToPixelShader;
	ComPtr<ID3D11Buffer>     Buffer;
	JArray<FCBufferVariable> Variables;

	void GenBuffer(ID3D11Device* InDevice)
	{
		if (Buffer.Get())
		{
			Buffer = nullptr;
		}
		Utils::DX::CreateBuffer(
								InDevice,
								D3D11_BIND_CONSTANT_BUFFER,
								nullptr,
								Size,
								1,
								Buffer.GetAddressOf(),
								D3D11_USAGE_DYNAMIC,
								D3D11_CPU_ACCESS_WRITE
							   );
	}

	void SetConstantBuffer(ID3D11DeviceContext* InDeviceContext)
	{
		if (Buffer.Get())
		{
			if (bPassToVertexShader)
			{
				InDeviceContext->VSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
			}

			if (bPassToPixelShader)
			{
				InDeviceContext->PSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
			}
		}
	}

	template <typename BufferData>
	void UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const BufferData& InData)
	{
		Utils::DX::UpdateDynamicBuffer(
									   InDeviceContext,
									   Buffer.Get(),
									   &InData,
									   sizeof(BufferData));
	}


	JConstantBuffer() = default;

	JConstantBuffer(const JText& InName,
					uint32_t     InSize,
					uint32_t     InSlot                = 0,
					bool         bInPassToVertexShader = false,
					bool         bInPassToPixelShader  = false)
		: Name(InName),
		  Size(InSize),
		  Slot(InSlot),
		  bPassToVertexShader(bInPassToVertexShader),
		  bPassToPixelShader(bInPassToPixelShader)
	{
		Hash   = StringHash(Name.c_str());
		Buffer = nullptr;
		Variables.clear();
	}
};

#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/Graphics/DXUtils.h"

enum EConstantFlags : uint8_t
{
	PassNone     = 0,
	PassVertex   = 1 << 0,
	PassPixel    = 1 << 1,
	PassGeometry = 1 << 2,
	PassHull     = 1 << 3,
	PassDomain   = 1 << 4,
	PassCompute  = 1 << 5,
	PassAll      = 0xFFFFFFFF
};

struct FCBufferVariable
{
	JText    Name;
	uint32_t Hash;
	uint32_t Offset;
	uint32_t Size;

	FCBufferVariable() = default;

	FCBufferVariable(const JText& InName, uint32_t InOffset, uint32_t InSize)
		: Name(InName),
		  Offset(InOffset),
		  Size(InSize)
	{
		Hash = StringHash(Name.c_str());
	}

	bool operator==(const FCBufferVariable& InOther) const
	{
		return Hash == InOther.Hash;
	}
};

DECLARE_DYNAMIC_DELEGATE(FOnConstantBufferPropertyChanged,
						 ID3D11DeviceContext* InDeviceContext,
						 ID3D11Buffer* InBuffer,
						 void* InData,
						 uint32_t InSize)

struct FConstantBuffer
{
	JText                    Name;
	uint32_t                 Hash;
	uint32_t                 Size;
	uint32_t                 Slot;
	uint8_t                  Flags;
	ComPtr<ID3D11Buffer>     Buffer;
	JArray<FCBufferVariable> Variables;
	JHash<uint32_t, int32_t> VariableHashTable;
	JArray<uint8_t>          Data;

	FOnConstantBufferPropertyChanged OnConstantBufferPropertyChanged;

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

		// 이벤트 등록
		OnConstantBufferPropertyChanged.Bind([&](ID3D11DeviceContext* InDeviceContext, ID3D11Buffer* InBuffer,
												 void*                InData, uint32_t               InSize){
			Utils::DX::UpdateDynamicBuffer(
										   InDeviceContext,
										   InBuffer,
										   InData,
										   InSize);
		});
	}

	FCBufferVariable* GetVariable(const JText& InName)
	{
		uint32_t hash = StringHash(InName.c_str());
		if (VariableHashTable.contains(hash))
		{
			return &Variables[VariableHashTable[hash]];
		}

		return nullptr;
	}

	void SetConstantBuffer(ID3D11DeviceContext* InDeviceContext)
	{
		if (Buffer.Get())
		{
			if (Flags & EConstantFlags::PassVertex)
			{
				InDeviceContext->VSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
			}

			if (Flags & EConstantFlags::PassPixel)
			{
				InDeviceContext->PSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
			}
		}
	}

	void UpdateBuffer(ID3D11DeviceContext* InDeviceContext)
	{
		OnConstantBufferPropertyChanged.Execute(InDeviceContext, Buffer.Get(), Data.data(), Data.size());
	}

	FConstantBuffer() = default;

	FConstantBuffer(const JText& InName,
					uint32_t     InSize,
					uint32_t     InSlot                = 0,
					bool         bInPassToVertexShader = false,
					bool         bInPassToPixelShader  = false)
		: Name(InName),
		  Hash(StringHash(InName.c_str())),
		  Size(InSize),
		  Slot(InSlot),
		  Flags(0)
	{
		Buffer = nullptr;
		Variables.clear();
		if (bInPassToVertexShader)
		{
			Flags |= EConstantFlags::PassVertex;
		}
		if (bInPassToPixelShader)
		{
			Flags |= EConstantFlags::PassPixel;
		}
	}
};

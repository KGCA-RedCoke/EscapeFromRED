#include "common_pch.h"
#include "JSerialization.h"

#include "Core/Utils/Logger.h"
#include "Core/Utils/Utils.h"

size_t AutoSerializer::GetSerializationBufferSize() const
{
	size_t size{};
	size += sizeof(size_t);   // total size.
	size += sizeof(int);      // total count size.
	for (int i = 0; i < Serializables.Members.size(); ++i)
	{
		size += sizeof(int);    // index size.
		size += Serializables.Members[i]->GetDataSize();
	}

	return size;
}

size_t AutoSerializer::GetModifiedSerializationBufferSize() const
{
	size_t size{};
	size += sizeof(size_t);   // total size.
	size += sizeof(int);      // total count size.
	for (int i = 0; i < Serializables.Members.size(); ++i)
	{
		if (Serializables.Members[i]->IsModified())
		{
			size += sizeof(int);    // index size.
			size += Serializables.Members[i]->GetDataSize();
		}
	}

	return size;
}

void AutoSerializer::Serialize(char* Buffer)
{
	// total size.
	size_t totalSize{};
	totalSize += sizeof(size_t);
	char* ptrTotalSize = Buffer;
	Buffer += sizeof(totalSize);

	// total count.
	int   totalCount{};
	char* ptrTotalCount = Buffer;
	Buffer += sizeof(totalCount);
	totalSize += sizeof(totalCount);

	// members.
	for (int i = 0; i < Serializables.Members.size(); ++i)
	{
		ISerializable* p = Serializables.Members[i];

		// position of vector.
		memcpy_s(Buffer, sizeof(i), &i, sizeof(i));
		Buffer += sizeof(i);
		totalSize += sizeof(i);

		// data.
		memcpy_s(Buffer, p->GetDataSize(), p->GetData(), p->GetDataSize());
		Buffer += p->GetDataSize();
		totalSize += p->GetDataSize();

		++totalCount;

	}

	// total count.
	memcpy_s(ptrTotalCount, sizeof(totalCount), &totalCount, sizeof(totalCount));

	// total size.
	memcpy_s(ptrTotalSize, sizeof(ptrTotalSize), &totalSize, sizeof(ptrTotalSize));

	// Set 'isModified' state to false after serialized once.
	ResetModifiedState();

	JText         fileName = "Game/test.txt";
	std::ofstream outFile(fileName, std::ios::binary);
	outFile.write(Buffer, totalSize);
}

void AutoSerializer::Serialize(std::ofstream& OutFileStream) const
{
	JAssetHeader   header;
	JAssetMetaData metadata;

	header.DataSize        = 0;
	
	// header.DataSize += (sizeof(JAssetHeader) + sizeof(JAssetMetaData) + sizeof(metadata.InstanceCount));

	OutFileStream.write(reinterpret_cast<char*>(&header), sizeof(JAssetHeader));

	OutFileStream.write(reinterpret_cast<char*>(&metadata), sizeof(JAssetMetaData));

	for (int32_t i = 0; i < Serializables.Members.size(); ++i)
	{
		ISerializable* variable = Serializables.Members[i];

		OutFileStream.write(reinterpret_cast<char*>(&i), sizeof(i));
		header.DataSize += sizeof(i);

		void*    data     = variable->GetData();
		uint32_t dataSize = variable->GetDataSize();

		OutFileStream.write(static_cast<char*>(data), dataSize);
		header.DataSize += dataSize;

		// ++metadata.InstanceCount;
	}

	OutFileStream.seekp(0);

	OutFileStream.write(reinterpret_cast<char*>(&header), sizeof(JAssetHeader));
	OutFileStream.write(reinterpret_cast<char*>(&metadata), sizeof(JAssetMetaData));

	OutFileStream.flush();
	OutFileStream.close();
}

void AutoSerializer::DeSerialize(const char* Buffer) const
{
	// total size.
	size_t totalSize{};
	memcpy_s(&totalSize, sizeof(totalSize), Buffer, sizeof(totalSize));
	Buffer += sizeof(totalSize);

	// total count.
	int totalCount{};
	memcpy_s(&totalCount, sizeof(totalCount), Buffer, sizeof(totalCount));
	Buffer += sizeof(totalCount);

	// Members.
	for (int i = 0; i < totalCount; ++i)
	{
		// position of vector.
		int index{};
		memcpy_s(&index, sizeof(index), Buffer, sizeof(index));
		Buffer += sizeof(index);

		// Data.
		ISerializable* p = Serializables.Members[index];
		memcpy_s(p->GetData(), p->GetDataSize(), Buffer, p->GetDataSize());
		Buffer += p->GetDataSize();
	}
}

void AutoSerializer::DeSerialize(std::ifstream& InFileStream) const
{
	JAssetHeader   header{};
	JAssetMetaData metadata{};

	InFileStream.read(reinterpret_cast<char*>(&header), sizeof(header));
	InFileStream.read(reinterpret_cast<char*>(&metadata), sizeof(metadata));

	if (StringHash(header.Signature) != JAssetHash)
	{
		LOG_CORE_ERROR("Invalid jasset header");

		InFileStream.close();
		return;
	}

	for (int32_t i = 0; i < header.InstanceCount; ++i)
	{
		int32_t index{};
		InFileStream.read(reinterpret_cast<char*>(&index), sizeof(index));

		ISerializable* dataStream = Serializables.Members[index];
		InFileStream.read(static_cast<char*>(dataStream->GetData()),
						  static_cast<long long>(dataStream->GetDataSize()));
	}
}

void AutoSerializer::ResetModifiedState()
{
	for (int i = 0; i < Serializables.Members.size(); ++i)
	{
		Serializables.Members[i]->ResetState();
	}
}

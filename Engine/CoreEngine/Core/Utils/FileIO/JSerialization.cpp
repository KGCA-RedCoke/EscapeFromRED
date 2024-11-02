#include "JSerialization.h"

#include "Core/Utils/Logger.h"
#include "Core/Utils/Utils.h"

namespace Utils::Serialization
{

	JAssetMetaData GetType(const char* InFilePath)
	{
		std::ifstream archive(InFilePath, std::ios::binary);
		if (!archive.is_open())
			return JAssetMetaData{};

		JAssetHeader   header;
		JAssetMetaData metaData;

		DeserializeHeader(archive, header);
		DeSerialize_Primitive(&metaData, sizeof(metaData), archive);

		return metaData;
	}

	bool SerializeHeader(std::ofstream& FileStream)
	{
		if (FileStream.is_open())
		{
			FileStream.seekp(0, std::ios::beg);

			JAssetHeader header;

			Serialize_Primitive(&header, sizeof(header), FileStream);
			return true;
		}

		return false;
	}

	bool DeserializeHeader(std::ifstream& InFileStream, JAssetHeader& OutHeader)
	{
		if (InFileStream.is_open())
		{
			InFileStream.seekg(0, std::ios::beg);

			DeSerialize_Primitive(&OutHeader, sizeof(OutHeader), InFileStream);
			return true;
		}
		ShowErrorPopup("File is not opened.");
		return false;
	}

	bool SerializeMetaData(std::ofstream& FileStream, const ISerializable* InData)
	{
		if (FileStream.is_open())
		{
			JAssetMetaData metaData;
			metaData.AssetType     = InData->GetType();
			metaData.DataSize      = 0;
			metaData.InstanceCount = 0;

			Serialize_Primitive(&metaData, sizeof(metaData), FileStream);

			return true;
		}
		return false;
	}

	bool DeserializeMetaData(std::ifstream& InFileStream, JAssetMetaData& OutMetaData, uint32_t InType)
	{
		if (InFileStream.is_open())
		{
			DeSerialize_Primitive(&OutMetaData, sizeof(OutMetaData), InFileStream);

			return OutMetaData.AssetType == InType;

		}
		ShowErrorPopup("File is not opened.");
		return false;
	}

	bool CheckAssetType(std::ifstream& InFileStream, uint32_t InType)
	{
		if (InFileStream)
		{
			InFileStream.seekg(sizeof(JAssetHeader), std::ios::beg);

			JAssetMetaData metaData;
			return DeserializeMetaData(InFileStream, metaData, InType);
		}

		return false;
	}

	bool Serialize(const char* InFilePath, ISerializable* InData)
	{
		std::ofstream archive(InFilePath, std::ios::binary);
		if (!archive.is_open())
			return false;

		archive.clear();

		SerializeHeader(archive);

		if (InData->Serialize_Implement(archive))
		{
			archive.flush();
			archive.close();
			return true;
		}

		return false;
	}

	bool DeSerialize(const char* InFilePath, ISerializable* OutData)
	{
		std::ifstream archive(InFilePath, std::ios::binary);
		if (!archive.is_open())
			return false;

		JAssetHeader header;

		DeserializeHeader(archive, header);

		if (OutData->DeSerialize_Implement(archive))
		{
			archive.close();
			return true;
		}
		return false;
	}

	void Serialize_Text(JText& Text, std::ofstream& FileStream)
	{
		size_t textSize = Text.size();
		FileStream.write(reinterpret_cast<char*>(&textSize), sizeof(textSize));
		FileStream.write(reinterpret_cast<char*>(Text.data()), textSize);
	}

	void Serialize_Text(JWText& Text, std::ofstream& FileStream)
	{
		JText text = WString2String(Text);
		Serialize_Text(text, FileStream);
	}

	void Serialize_Primitive(void* Data, size_t Size, std::ofstream& FileStream)
	{
		FileStream.write(reinterpret_cast<char*>(Data), Size);
	}

	void Serialize_Container(void* Data, size_t Size, std::ofstream& FileStream)
	{
		// 컨테이너 사이즈
		Serialize_Primitive(&Size, sizeof(Size), FileStream);

		// 컨테이너 데이터
		for (size_t i = 0; i < Size; ++i)
		{
			if constexpr (bIsSerializable<decltype(Data)>)
			{
				auto* serializable = static_cast<ISerializable*>(Data);
				serializable->Serialize_Implement(FileStream);
			}
			else
			{
				Serialize_Primitive(Data, sizeof(decltype(Data)), FileStream);
			}
		}
	}

	void DeSerialize_Text(JText& Text, std::ifstream& InFileStream)
	{
		size_t textSize;
		InFileStream.read(reinterpret_cast<char*>(&textSize), sizeof(textSize));
		Text.resize(textSize);
		InFileStream.read(reinterpret_cast<char*>(Text.data()), textSize);
	}

	void DeSerialize_Text(JWText& Text, std::ifstream& InFileStream)
	{
		JText text;
		DeSerialize_Text(text, InFileStream);
		Text = String2WString(text);
	}


	void DeSerialize_Container(void* Data, std::ifstream& InFileStream)
	{
		// 컨테이너 사이즈
		size_t containerSize;
		DeSerialize_Primitive(&containerSize, sizeof(containerSize), InFileStream);

		// 컨테이너 데이터
		for (size_t i = 0; i < containerSize; ++i)
		{
			if constexpr (bIsSerializable<decltype(Data)>)
			{
				auto* serializable = static_cast<ISerializable*>(Data);
				serializable->DeSerialize_Implement(InFileStream);
			}
			else
			{
				DeSerialize_Primitive(Data, sizeof(decltype(Data)), InFileStream);
			}
		}
	}


	void DeSerialize_Primitive(void* Data, size_t Size, std::ifstream& InFileStream)
	{
		InFileStream.read(reinterpret_cast<char*>(Data), Size);
	}
}

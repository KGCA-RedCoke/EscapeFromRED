#pragma once

/**
 * 컴파일타임에 Serialize 메서드가 있는지 확인
 */
template <typename T>
class HasSerializeMethod
{
	// SFINAE (Substitution Failure Is Not An Error)원리
	// T에 Serialize 메서드가 있으면 첫번째 함수가 호출되고 없으면 두번째 함수가 호출된다.
	// template <typename C> static std::true_type  Has(decltype(&C::Serialize)); // 이건 메서드 타입이 여러개면 컴파일러가 헷갈려 함
	template <typename C> static auto            Has(int) -> decltype(std::declval<C>().Serialize(), std::true_type{});
	template <typename C> static std::false_type Has(...);

public:
	// enum { value = decltype(Has<T>(0))::value }; // 꼼수인데 모던 C++을 쓰자...
	static constexpr bool value = decltype(Has<T>(0))::value; // 좀 더 코드를 볼 때 명확한듯
};

template <typename T> struct IsSmartPointer : std::false_type
{};

template <typename T> struct IsSmartPointer<std::shared_ptr<T>> : std::true_type
{};

template <typename T> struct IsSmartPointer<std::unique_ptr<T>> : std::true_type
{};

// 벡터 타입인지 확인
template <typename T> struct IsVector : std::false_type
{};

template <typename T, typename Alloc> struct IsVector<std::vector<T, Alloc>> : std::true_type
{};

// 문자열 타입인지 확인
template <typename T> struct IsString : std::false_type
{};

template <> struct IsString<std::string> : std::true_type
{};

// 맵 타입인지 확인
template <typename T> struct IsMap : std::false_type
{};

template <typename Key, typename T, typename Compare, typename Alloc> struct IsMap<std::unordered_map<
	Key, T, Compare, Alloc>> : std::true_type
{};

// 배열 타입인지 확인
template <typename T> struct IsArray : std::false_type
{};

template <typename T, std::size_t N> struct IsArray<T[N]> : std::true_type
{};

template <typename T> struct IsArray<T[]> : std::true_type
{};


template <typename T> constexpr bool bIsIntegralType     = std::is_integral_v<T>;
template <typename T> constexpr bool bIsEnumType         = std::is_enum_v<T>;
template <typename T> constexpr bool bIsPlainOldDataType = std::is_pod_v<T>;
template <typename T> constexpr bool bIsPtr              = std::is_pointer_v<T>;
template <typename T> constexpr bool bIsSmartPtr         = IsSmartPointer<T>::value;
template <typename T> constexpr bool bIsSerializable     = HasSerializeMethod<T>::value;
template <typename T> constexpr bool bIsString           = IsString<T>::value;
template <typename T> constexpr bool bIsMap              = IsMap<T>::value;
template <typename T> constexpr bool bIsArray            = IsArray<T>::value;
template <typename T> constexpr bool bIsVector           = IsVector<T>::value;

/**
 * 직렬화/역직렬화 할 때 사용하는 헤더
 */
struct JAssetHeader
{
	char     Signature[8] = "JASSET\0";	// 파일 식별자		
	uint32_t Version      = 1;			// 엔진 버전
	uint32_t DataSize;					// 데이터 크기
	uint32_t InstanceCount;				// 인스턴스(멤버) 개수
};

class ISerializable
{
public:
	virtual void   Serialize(std::ofstream& FileStream) = 0;
	virtual void   DeSerialize(std::ifstream& InFileStream) = 0;
};

class SerializableMember
{
public:
	std::vector<ISerializable*> Members{};

	void AddMember(ISerializable* element)
	{
		Members.push_back(element);
	}
};

class AutoSerializer
{
public:
	/** 멤버 자동 등록 */
	void AddMember(ISerializable* member)
	{
		Serializables.AddMember(member);
	}

	// // Get size of total buffer.
	// size_t GetSerializationBufferSize() const;
	//
	// // Get size of modified Members only.
	// size_t GetModifiedSerializationBufferSize() const;

	// // Serailize.
	// void Serialize(char* Buffer);
	//
	// void Serialize(std::ofstream& OutFileStream) const;
	//
	// /**
	//  * 역 직렬화 메서드
	//  * 메모리에서 직접 버퍼를 읽음
	//  */
	// void DeSerialize(const char* Buffer) const;
	//
	// /**
	//  * 역 직렬화 메서드
	//  * 파일스트림에서 버퍼를 읽음 (더 느림!)
	//  */
	// void DeSerialize(std::ifstream& InFileStream) const;

	// void ResetModifiedState();

private:
	SerializableMember Serializables{};
};

/**
 * \class JProperty
 * ArrayContainer의 vector에 Serialize목록을 넘겨줘야 해서 공통된 인터페이스 작성
 */
template <typename T>
class JProperty : public ISerializable
{
public:
	JProperty() = delete;

public:
	explicit JProperty(AutoSerializer* Serializer)
	{
		Serializer->AddMember(this);
	}

	T Get() const
	{
		return mData;
	}

	T Get()
	{
		return mData;
	}

	void Set(T Val)
	{
		mData       = Val;
		bIsModified = true;
	}

	T* GetPtr()
	{
		return &mData;
	}

	void Serialize(std::ofstream& FileStream) override
	{
		if constexpr (bIsMap<T>)
		{
			for (auto& [key, value] : mData)
			{
				FileStream.write(static_cast<char*>(&key), sizeof(key));
				FileStream.write(static_cast<char*>(&value), sizeof(value));
			}
		}
		// else
		// {
		// 	FileStream.write(static_cast<char*>(GetData()), GetDataSize());
		// }
	}

	// void* GetData() override
	// {
	// 	if constexpr (bIsSmartPtr<T>)
	// 	{
	// 		return mData.get();
	// 	}
	//
	// 	if constexpr (bIsPtr<T>)
	// 	{
	// 		return mData;
	// 	}
	//
	// 	if constexpr (bIsVector<T>)
	// 	{
	// 		return mData.data();
	// 	}
	//
	// 	return &mData;
	// }
	//
	// size_t GetDataSize() override
	// {
	// 	if constexpr (bIsSmartPtr<T> || bIsPtr<T>)
	// 	{
	// 		return sizeof(*mData);
	// 	}
	//
	// 	if constexpr (bIsString<T>)
	// 	{
	// 		return mData.length();
	// 	}
	//
	// 	if constexpr (bIsVector<T> || bIsArray<T> || bIsMap<T>)
	// 	{
	// 		return mData.size() * sizeof(T);
	// 	}
	//
	// 	return sizeof(mData);
	// }
	//
	// bool IsModified() override
	// {
	// 	return bIsModified;
	// }
	//
	// void ResetState() override
	// {
	// 	this->bIsModified = false;
	// }
	//
	// JProperty& operator=(T& Val)
	// {
	// 	if (mData == Val)
	// 		return *this;
	//
	// 	mData = Val;
	// 	return *this;
	// }

private:
	T    mData;
	bool bIsModified = false;
};

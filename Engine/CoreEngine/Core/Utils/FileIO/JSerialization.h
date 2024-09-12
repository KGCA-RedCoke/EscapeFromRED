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


template <typename T> constexpr bool bIsIntegralType     = std::is_integral_v<T>;
template <typename T> constexpr bool bIsEnumType         = std::is_enum_v<T>;
template <typename T> constexpr bool bIsPlainOldDataType = std::is_pod_v<T>;
template <typename T> constexpr bool bIsSerializable     = HasSerializeMethod<T>::value;

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

struct JAssetMetaData
{};

class ISerializable
{
public:
	virtual void   Serialize() = 0;
	virtual void*  GetData() = 0;
	virtual size_t GetDataSize() = 0;
	virtual bool   IsModified() = 0;
	virtual void   ResetState() = 0;
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

	// Get size of total buffer.
	size_t GetSerializationBufferSize() const;

	// Get size of modified Members only.
	size_t GetModifiedSerializationBufferSize() const;

	void Serialize() {}

	// Serailize.
	void Serialize(char* Buffer);

	void Serialize(std::ofstream& OutFileStream) const;

	/**
	 * 역 직렬화 메서드
	 * 메모리에서 직접 버퍼를 읽음
	 */
	void DeSerialize(const char* Buffer) const;

	/**
	 * 역 직렬화 메서드
	 * 파일스트림에서 버퍼를 읽음 (더 느림!)
	 */
	void DeSerialize(std::ifstream& InFileStream) const;

	void ResetModifiedState();

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
	JProperty(AutoSerializer* serializer)
	{
		serializer->AddMember(this);
	}

	bool IsValid() const
	{
		return Data != NULL;
	}

	T Get() const
	{
		return Data;
	}

	T Get()
	{
		return Data;
	}

	void Set(T val)
	{
		Data        = val;
		bIsModified = true;
	}

	T* GetPtr()
	{
		return &Data;
	}

	void Serialize() override
	{}

	void* GetData() override
	{
		return &Data;
	}

	size_t GetDataSize() override
	{
		return sizeof(Data);
	}

	bool IsModified() override
	{
		return bIsModified;
	}

	void ResetState() override
	{
		this->bIsModified = false;
	}

	void operator=(T val)
	{
		Data = val;
	}

private:
	T    Data{};
	bool bIsModified{};
};

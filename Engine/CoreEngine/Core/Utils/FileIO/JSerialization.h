#pragma once
#include "common_include.h"
#include "Core/Utils/Utils.h"
#include "Debug/Assert.h"
/**
 * 컴파일타임에 Serialize_Implement 메서드가 있는지 확인
 */
template <typename T>
class HasSerializeMethod
{
	// SFINAE (Substitution Failure Is Not An Error)원리
	// T에 Serialize_Implement 메서드가 있으면 첫번째 함수가 호출되고 없으면 두번째 함수가 호출된다.
	// template <typename C> static std::true_type  Has(decltype(&C::Serialize_Implement)); // 이건 메서드 타입이 여러개면 컴파일러가 헷갈려 함
	template <typename C> static auto Has(int) -> decltype(std::declval<C>().Serialize_Implement(), std::true_type{});
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

const uint32_t SignatureHash = StringHash("JASSET");

/**
 * 직렬화/역직렬화 할 때 사용하는 헤더
 */
struct JAssetHeader
{
	char     Signature[8] = "JASSET\0";	// 파일 식별자		
	uint32_t Version      = 2;			// 엔진 버전
};

/**
 * 직렬화/역직렬화 할 때 사용하는 메타데이터
 */
struct JAssetMetaData
{
	uint32_t AssetType;		// 클래스 타입 (지금은 이것만 사용)
	uint32_t DataSize;		// 데이터 크기 (확장성을 위해 사용)
	uint32_t InstanceCount;	// 인스턴스 개수 (확장성을 위해 사용)
};

class ISerializable
{
public:
	virtual ~ISerializable() = default;

public:
	virtual uint32_t GetType() const = 0;
	virtual bool     Serialize_Implement(std::ofstream& FileStream) = 0;
	virtual bool     DeSerialize_Implement(std::ifstream& InFileStream) = 0;
};

class JAsset : public ISerializable
{
public:
	FORCEINLINE JText GetPath() const { return mPath; }

protected:
	JText mPath;
};

namespace Utils::Serialization
{
	bool IsJAssetFileAndExist(const char* InFilePath);

	JAssetMetaData GetType(const char* InFilePath);

	bool SerializeHeader(std::ofstream& FileStream);
	bool DeserializeHeader(std::ifstream& InFileStream, JAssetHeader& OutHeader);
	bool SerializeMetaData(std::ofstream& FileStream, const ISerializable* InData);
	bool DeserializeMetaData(std::ifstream& InFileStream, JAssetMetaData& OutMetaData, uint32_t InType);

	bool CheckAssetType(std::ifstream& InFileStream, uint32_t InType);

	bool Serialize(const char* InFilePath, class ISerializable* InData);
	bool DeSerialize(const char* InFilePath, ISerializable* OutData);

	void Serialize_Text(JText& Text, std::ofstream& FileStream);
	void Serialize_Text(JWText& Text, std::ofstream& FileStream);
	void Serialize_Primitive(void* Data, size_t Size, std::ofstream& FileStream);
	void Serialize_Container(void* Data, size_t Size, std::ofstream& FileStream);

	void DeSerialize_Primitive(void* Data, size_t Size, std::ifstream& InFileStream);
	void DeSerialize_Text(JText& Text, std::ifstream& InFileStream);
	void DeSerialize_Text(JWText& Text, std::ifstream& InFileStream);

	/**
	 * 사용 X Data를 공통적으로 초기화 할 수 있는 방법을 찾아내야한다.
	 */
	[[deprecated]] void DeSerialize_Container(void* Data, std::ifstream& InFileStream);
}

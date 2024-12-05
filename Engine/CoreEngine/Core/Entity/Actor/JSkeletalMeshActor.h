#pragma once
#include "AActor.h"

class JSkeletalMeshComponent;

class JSkeletalMeshActor : public AActor
{

public:
	JSkeletalMeshActor();

	/**
	 * 단순한 메시 오브젝트로부터 액터 생성
	 * @param InName 오브젝트 고유 네임
	 * @param SavedMeshPath 저장된 메시 경로(jasset) 
	 */
	JSkeletalMeshActor(JTextView InName, JTextView SavedMeshPath);
	JSkeletalMeshActor(const JSkeletalMeshActor& Copy);

	UPtr<IManagedInterface> Clone() const override;

public:
	void Initialize() override;

public:
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	int32_t GetMaterialCount() const;
	void    SetMaterialInstance(class JMaterialInstance* InMaterialInstance, uint32_t InIndex = 0);

private:
	void CreateMeshComponent(JTextView InMeshObject);

private:
	JSkeletalMeshComponent* mSkeletalMeshComponent = nullptr;
};

REGISTER_CLASS_TYPE(JSkeletalMeshActor);

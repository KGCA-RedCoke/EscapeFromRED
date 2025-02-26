﻿#pragma once
#include "AActor.h"

class JStaticMeshComponent;
class JMeshObject;

class JStaticMeshActor : public AActor
{
public:
	JStaticMeshActor();

	/**
	 * 단순한 메시 오브젝트로부터 액터 생성
	 * @param InName 오브젝트 고유 네임
	 * @param SavedMeshPath 저장된 메시 경로(jasset) 
	 */
	JStaticMeshActor(JTextView InName, JTextView SavedMeshPath);
	JStaticMeshActor(const JStaticMeshActor& Copy);

	UPtr<IManagedInterface> Clone() const override;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;

public:
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

	void Draw() override;
public:
	void ShowEditor() override;

public:
	int32_t GetMaterialCount() const;
	void    SetMaterialInstance(class JMaterialInstance* InMaterialInstance, uint32_t InIndex = 0);

private:
	void CreateMeshComponent(JTextView InMeshObject);

private:
	JStaticMeshComponent* mStaticMeshComponent = nullptr;
	bool                  bUseMeshCollision    = false;

	struct
	{
		bool    bChunkMesh    = false;
		int32_t mChunkSize    = 1000;
		float   AreaSize      = 3000;
		float   NoiseStrength = 100.f;
	} mMeshChunkData;
};

REGISTER_CLASS_TYPE(JStaticMeshActor);

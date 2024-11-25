#pragma once
#include "Core/Entity/Component/Scene/JSceneComponent.h"

class JSkeletalMeshObject;

class JSkeletalMeshComponent : public JSceneComponent
{
public:
	JSkeletalMeshComponent();
	JSkeletalMeshComponent(JTextView        InName,
						   JActor*          InOwnerActor      = nullptr,
						   JSceneComponent* InParentComponent = nullptr);
	~JSkeletalMeshComponent() override = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void SetSkeletalMesh(JTextView InSkeletalMeshPath);

private:
	UPtr<JSkeletalMeshObject> mSkeletalMeshObject = nullptr;
};

#pragma once
#include "Core/Entity/Component/Scene/JSceneComponent.h"

class JAnimationClip;
class JAnimator;
class JSkeletalMeshObject;

class JSkeletalMeshComponent : public JSceneComponent
{
public:
	JSkeletalMeshComponent();
	JSkeletalMeshComponent(JTextView        InName,
						   AActor*          InOwnerActor      = nullptr,
						   JSceneComponent* InParentComponent = nullptr);
	~JSkeletalMeshComponent() override = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Tick(float DeltaTime) override;

	void Draw() override;
	void DrawID(uint32_t ID) override;

public:
	void SetSkeletalMesh(JTextView InSkeletalMeshPath);
	void SetAnimation(JAnimationClip* InAnimationClip);

public:
	void ShowEditor() override;

private:
	UPtr<JSkeletalMeshObject> mSkeletalMeshObject = nullptr;
	UPtr<JAnimator>           mAnimator           = nullptr;
};

REGISTER_CLASS_TYPE(JSkeletalMeshComponent);

#pragma once

#include "Core/Entity/Component/Scene/JSceneComponent.h"
#include "Core/Entity/Component/Scene/Shape/JShape.h"
#include "Core/Graphics/ShaderStructs.h"

class JMeshObject;

class JStaticMeshComponent : public JSceneComponent
{
public:
	JStaticMeshComponent();
	JStaticMeshComponent(JTextView        InName,
						 AActor*          InOwnerActor           = nullptr,
						 JSceneComponent* InParentSceneComponent = nullptr);
	JStaticMeshComponent(const JStaticMeshComponent& Copy);
	~JStaticMeshComponent() override;

public:
	uint32_t GetType() const override { return StringHash("StaticMeshComponent"); }
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Tick(float DeltaTime) override;

	void Draw() override;
	void DrawID(uint32_t ID) override;

public:
	void SetMaterialInstance(class JMaterialInstance* InMaterialInstance, uint32_t InSlot = 0);
	void SetMeshObject(JTextView InMeshObject);

	int32_t GetMaterialCount() const;

public:
	void ShowEditor() override;

private:
	UPtr<JMeshObject> mMeshObject = nullptr;
};

REGISTER_CLASS_TYPE(JStaticMeshComponent);

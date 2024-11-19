#pragma once

#include "Core/Entity/Component/Scene/JSceneComponent.h"
#include "Core/Graphics/ShaderStructs.h"

class JMeshObject;

class JStaticMeshComponent : public JSceneComponent
{
public:
	JStaticMeshComponent();
	JStaticMeshComponent(JTextView        InName,
						 JActor*          InOwnerActor           = nullptr,
						 JSceneComponent* InParentSceneComponent = nullptr);
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

public:
	void SetMeshObject(JTextView InMeshObject);

protected:
	UPtr<JMeshObject> mMeshObject = nullptr;
};

REGISTER_CLASS_TYPE(JStaticMeshComponent);

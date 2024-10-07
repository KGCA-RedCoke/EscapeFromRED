#pragma once

#include "Core/Entity/Component/Scene/JSceneComponent.h"
#include "Core/Graphics/ShaderStructs.h"

class JMeshObject;

class JStaticMeshComponent : public JSceneComponent
{
public:
	JStaticMeshComponent();
	JStaticMeshComponent(JTextView InName);
	~JStaticMeshComponent() override;

public:
	EObjectType GetObjectType() const override { return EObjectType::SceneComponent; }
	uint32_t    GetType() const override { return StringHash("StaticMeshComponent"); }

public:
	void Tick(float DeltaTime) override;

	void Draw() override;

public:
	FORCEINLINE void         SetMeshObject(JMeshObject* InMeshObject) { mMeshObject = InMeshObject; }
	FORCEINLINE JMeshObject* GetMeshObject() const { return mMeshObject; }

protected:
	JMeshObject* mMeshObject = nullptr;
};

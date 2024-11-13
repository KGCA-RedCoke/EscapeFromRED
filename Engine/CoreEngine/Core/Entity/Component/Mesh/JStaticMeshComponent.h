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

	void Draw(ID3D11DeviceContext* InDeviceContext) override;
	void DrawID(ID3D11DeviceContext* InDeviceContext, uint32_t ID) override;

public:
	FORCEINLINE void             SetMeshObject(const Ptr<JMeshObject>& InMeshObject) { mMeshObject = InMeshObject; }
	FORCEINLINE Ptr<JMeshObject> GetMeshObject() const { return mMeshObject; }

protected:
	Ptr<JMeshObject> mMeshObject = nullptr;
};

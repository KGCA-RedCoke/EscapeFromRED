#pragma once
#include "Core/Entity/Component/Scene/JSceneComponent.h"
#include "Core/Graphics/ShaderStructs.h"

class JLightComponent : public JSceneComponent
{
public:
	JLightComponent();
	JLightComponent(JTextView InName, AActor* InOwnerActor = nullptr, JSceneComponent* InParentComponent = nullptr);

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;

public:
	uint32_t GetType() const override;

public:
	void ShowEditor() override;

protected:
	uint32_t mWorldBufferIndex;
};

REGISTER_CLASS_TYPE(JLightComponent)

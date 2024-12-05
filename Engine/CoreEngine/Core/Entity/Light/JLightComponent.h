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
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void ShowEditor() override;

public:
	void SetLightColor(const FVector4& InColor) { mLightData.Color = InColor; }
	void SetRange(float InRange) { mLightData.Range = InRange; }
	void SetIntensity(float InIntensity) { mLightData.Intensity = InIntensity; }

	[[nodiscard]] const FVector& GetLightColor() const { return mLightData.Color; }
	[[nodiscard]] float          GetRange() const { return mLightData.Range; }
	[[nodiscard]] float          GetIntensity() const { return mLightData.Intensity; }

protected:
	Buffer::FBuffer_Light_Point mLightData;
	uint32_t                    mWorldBufferIndex;
};

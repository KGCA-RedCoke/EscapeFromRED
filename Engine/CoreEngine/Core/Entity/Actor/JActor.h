#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Entity/Component/Scene/JSceneComponent.h"


class JActorComponent;
class JTransformComponent;

class JActor : public JSceneComponent
{
public:
	JActor();
	JActor(JTextView InName);
	~JActor() override;

	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

	void Draw() override;

	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

	Ptr<JSceneComponent> GetRootComponent() const { return mRootComponent; }

protected:
	Ptr<JSceneComponent> mRootComponent;

	WPtr<JActor>             mParentActor;
	JArray<Ptr<JActor>>      mChildActors;
	JArray<JActorComponent*> mActorComponents;

};

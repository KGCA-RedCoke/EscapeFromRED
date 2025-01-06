#pragma once
#include "JStaticMeshComponent.h"

class JBillboardComponent : public JStaticMeshComponent
{
public:
	JBillboardComponent();
	JBillboardComponent(JTextView        InName,
						AActor*          InOwnerActor           = nullptr,
						JSceneComponent* InParentSceneComponent = nullptr);
	JBillboardComponent(const JBillboardComponent& Copy);
	~JBillboardComponent() override;

public:
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;
	
public:
	void Initialize() override;
	void Tick(float DeltaTime) override;

public:
	void ShowEditor() override;
	
};

REGISTER_CLASS_TYPE(JBillboardComponent);

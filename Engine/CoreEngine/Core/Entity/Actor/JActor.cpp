#include "JActor.h"

#include "Core/Interface/MManagerInterface.h"
#include "GUI/GUI_Inspector.h"


JActor::JActor() {}

JActor::JActor(JTextView InName)
	: JSceneComponent(InName)
{}

void JActor::Initialize()
{
	JSceneComponent::Initialize();

	Ptr<JActor> thisPtr = GetThisPtr<JActor>();

	mRootComponent = CreateDefaultSubObject<JSceneComponent>("RootComponent", thisPtr, thisPtr);
	mRootComponent->SetupAttachment(thisPtr);

	// TODO: Editor에서 Initialize 시에 호출되도록 수정
	// Case 1. 매크로 사용
	// Case 2. 생각 필요
	IManager.GUIManager->GetInspector()->AddSceneComponent(mName, thisPtr);
}

void JActor::BeginPlay()
{
	JSceneComponent::BeginPlay();
}

void JActor::Tick(float DeltaTime)
{
	JSceneComponent::Tick(DeltaTime);
}

void JActor::Destroy()
{
	JSceneComponent::Destroy();
}

void JActor::Draw()
{
	for (auto& sceneComponent : mChildSceneComponents)
	{
		if (auto ptr = sceneComponent.lock())
		{
			ptr->Draw();
		}
	}
}

bool JActor::Serialize_Implement(std::ofstream& FileStream)
{
	return JSceneComponent::Serialize_Implement(FileStream);
}

bool JActor::DeSerialize_Implement(std::ifstream& InFileStream)
{
	return JSceneComponent::DeSerialize_Implement(InFileStream);
}

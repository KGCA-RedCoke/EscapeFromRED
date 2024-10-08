#include "JActor.h"


JActor::JActor() {}

JActor::JActor(JTextView InName)
	: JSceneComponent(InName) {}

JActor::~JActor() {}

void JActor::Initialize()
{
	JSceneComponent::Initialize();
	Ptr<JActor> thisPtr = GetThisPtr<JActor>();
	mRootComponent      = CreateDefaultSubObject<JSceneComponent>("RootComponent", thisPtr, thisPtr);
	mRootComponent->SetupAttachment(thisPtr);
}

void JActor::BeginPlay()
{
	JSceneComponent::BeginPlay();
}

void JActor::Tick(float DeltaTime)
{
	JSceneComponent::Tick(DeltaTime);

	for (int32_t i = 0; i < mChildObjs.size(); ++i)
	{
		mChildObjs[i]->Tick(DeltaTime);
	}
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

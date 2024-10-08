#include "JSceneComponent.h"

#include "Core/Entity/Actor/JActor.h"

JSceneComponent::JSceneComponent() {}

JSceneComponent::JSceneComponent(JTextView InName)
	: JActorComponent(InName) {}

JSceneComponent::JSceneComponent(JTextView                   InName, const Ptr<JActor>& InOwnerActor,
								 const Ptr<JSceneComponent>& InParentSceneComponent)
	: JActorComponent(InName)
{
	mOwnerActor           = InOwnerActor;
	mParentSceneComponent = InParentSceneComponent;
}

JSceneComponent::~JSceneComponent() {}

bool JSceneComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JActorComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	// World Transform Data
	Utils::Serialization::Serialize_Primitive(&mWorldLocation, sizeof(FVector), FileStream);
	Utils::Serialization::Serialize_Primitive(&mWorldRotation, sizeof(FVector), FileStream);
	Utils::Serialization::Serialize_Primitive(&mWorldScale, sizeof(FVector), FileStream);

	// Local Transform Data
	Utils::Serialization::Serialize_Primitive(&mLocalLocation, sizeof(FVector), FileStream);
	Utils::Serialization::Serialize_Primitive(&mLocalRotation, sizeof(FVector), FileStream);
	Utils::Serialization::Serialize_Primitive(&mLocalScale, sizeof(FVector), FileStream);

	return true;
}

bool JSceneComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JActorComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	// World Transform Data
	Utils::Serialization::DeSerialize_Primitive(&mWorldLocation, sizeof(FVector), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mWorldRotation, sizeof(FVector), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mWorldScale, sizeof(FVector), InFileStream);

	// Local Transform Data
	Utils::Serialization::DeSerialize_Primitive(&mLocalLocation, sizeof(FVector), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mLocalRotation, sizeof(FVector), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mLocalScale, sizeof(FVector), InFileStream);

	return true;
}

void JSceneComponent::Tick(float DeltaTime)
{
	UpdateTransform();

	for (int32_t i = 0; i < mChildObjs.size(); ++i)
	{
		if (auto childComp = mChildObjs[i])
		{
			childComp->Tick(DeltaTime);
		}
	}
}

void JSceneComponent::Draw()
{
	for (int32_t i = 0; i < mChildSceneComponents.size(); ++i)
	{
		if (auto childComp = mChildSceneComponents[i].lock())
		{
			childComp->Draw();
		}
	}
}

void JSceneComponent::AddChildSceneComponent(const Ptr<JSceneComponent>& Ptr)
{
	mChildSceneComponents.push_back(Ptr);
}

void JSceneComponent::SetupAttachment(const Ptr<JSceneComponent>& InParentComponent)
{
	auto thisPtr = GetThisPtr<JSceneComponent>();

	if (!InParentComponent->mChildObjHash.contains(GetHash()))
	{
		mParentObj = InParentComponent;
		InParentComponent->mChildObjs.push_back(thisPtr);
		InParentComponent->mChildObjHash.insert({GetHash(), InParentComponent->mChildObjs.size()});
	}
	mParentSceneComponent = InParentComponent;
	InParentComponent->mChildSceneComponents.push_back(thisPtr);

}

void JSceneComponent::AttachComponent(const Ptr<JSceneComponent>& InChildComponent)
{
	assert(InChildComponent);

	InChildComponent->DetachFromComponent();

	InChildComponent->SetupAttachment(GetThisPtr<JSceneComponent>());
}

void JSceneComponent::AttachToComponent(const Ptr<JSceneComponent>& InParentComponent)
{
	assert(InParentComponent);

	Ptr<JActor> parentActor = InParentComponent->GetOwnerActor();
	assert(parentActor);

	DetachFromComponent();

	SetupAttachment(InParentComponent);
}

void JSceneComponent::AttachToActor(const Ptr<JActor>& InParentActor, const JText& InComponentAttachTo)
{
	assert(InParentActor);

	const uint32_t componentHash = StringHash(InComponentAttachTo.c_str());

	// 붙일 액터에 컴포넌트 유무 확인 (액터 내부에 최소한 RootComponent는 존재)
	assert(InParentActor->mChildObjHash.contains(componentHash));

	// 오브젝트 해시 테이블에서 컴포넌트 인덱스를 가져옴
	const int32_t index = InParentActor->mChildObjHash[componentHash];

	// 오브젝트를 씬 컴포넌트로 캐스팅
	const Ptr<JSceneComponent> componentToAttach = std::dynamic_pointer_cast<
		JSceneComponent>(InParentActor->mChildObjs[index]);

	assert(componentToAttach);

	// 부모 컴포넌트에 부착
	componentToAttach->AttachComponent(GetThisPtr<JSceneComponent>());
}

void JSceneComponent::AttachToActor(const Ptr<JActor>& InParentActor, const Ptr<JSceneComponent>& InComponentAttachTo)
{
	assert(InParentActor);


}

void JSceneComponent::DetachFromComponent()
{
	if (Ptr<JSceneComponent> parentPtr = mParentSceneComponent.lock())
	{
		// 부모 씬 컴포넌트의 자식 목록에서 제거
		auto it = std::ranges::find(parentPtr->mChildObjs, GetThisPtr<JSceneComponent>());
		if (it != parentPtr->mChildObjs.end())
		{
			parentPtr->mChildObjs.erase(it);
		}

		//  부모 씬 컴포넌트와의 연결을 해제
		mParentObj.reset();
		mParentSceneComponent.reset();
	}
}

void JSceneComponent::UpdateTransform()
{
	// Step1. 로컬 좌표 변환
	mLocalLocationMat = DirectX::XMMatrixTranslation(mLocalLocation.x, mLocalLocation.y, mLocalLocation.z);
	mLocalRotationMat = DirectX::XMMatrixRotationRollPitchYaw(
															  DirectX::XMConvertToRadians(mLocalRotation.x),
															  DirectX::XMConvertToRadians(mLocalRotation.y),
															  DirectX::XMConvertToRadians(mLocalRotation.z)
															 );
	mLocalScaleMat = DirectX::XMMatrixScaling(mLocalScale.x, mLocalScale.y, mLocalScale.z);

	mLocalMat = mLocalScaleMat * mLocalRotationMat * mLocalLocationMat;


	// Step2. 부모 씬 컴포넌트가 존재한다면, 로컬 행렬을 계산
	if (Ptr<JSceneComponent> parentPtr = mParentSceneComponent.lock())
	{
		mWorldMat = parentPtr->mWorldMat * mLocalMat;
	}
	else // 부모가 없는 씬 컴포넌트는 그 자체 위치를 월드 위치로 사용
	{
		mWorldMat = mLocalMat;
	}

	DirectX::XMVECTOR scale, rot, loc;
	XMMatrixDecompose(&scale, &rot, &loc, mWorldMat);
	mWorldLocation = loc;
	mWorldRotation = rot;
	mWorldScale    = scale;
}

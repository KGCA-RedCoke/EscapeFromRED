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
	JActorComponent::Tick(DeltaTime);

	UpdateTransform();
}

void JSceneComponent::Draw()
{
	for (int32_t i = 0; i < mChildSceneComponents.size(); ++i)
	{
		if (auto childComp = mChildSceneComponents[i])
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
	mParentSceneComponent = InParentComponent;
	InParentComponent->mChildSceneComponents.push_back(GetThisPtr<JSceneComponent>());
}

bool JSceneComponent::AttachSceneComponent(JSceneComponent* InParentComponent)
{
	assert(InParentComponent);

	// JActor* ownerActor = mOwnerActor;
	// // 컴포넌트는 항상 액터에 속해야 하므로, 액터가 존재하지 않으면 실패
	// assert(ownerActor);
	//
	// UPtr<JObject> objToAttach = nullptr;
	//
	// auto it = std::ranges::find_if(ownerActor->mChildObjs,
	// 							   [&](const auto& comp){
	// 								   return comp->GetHash() == GetHash();
	// 							   });
	//
	// if (it != ownerActor->mChildObjs.end())
	// {
	// 	objToAttach = std::move(*it);
	// 	ownerActor->mChildObjs.erase(it);
	//
	// 	JActor* newOwnerActor = InParentComponent->GetOwnerActor();
	// 	assert(newOwnerActor);
	//
	// 	newOwnerActor->mChildObjs.push_back(std::move(objToAttach));
	//
	// 	mOwnerActor = newOwnerActor;
	//
	// 	SetupAttachment(InParentComponent);
	// }
	// else
	// {
	// 	LOG_CORE_ERROR("Failed to attach scene component to actor");
	// 	return false;
	// }

	return true;
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

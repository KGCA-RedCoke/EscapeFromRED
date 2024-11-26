#include "JSceneComponent.h"

#include "Core/Entity/Actor/AActor.h"

JSceneComponent::JSceneComponent()
	: mParentSceneComponent(nullptr)
{
	mObjectType = NAME_OBJECT_SCENE_COMPONENT;
}

JSceneComponent::JSceneComponent(JTextView        InName, AActor* InOwnerActor,
								 JSceneComponent* InParentSceneComponent)
	: JActorComponent(InName)
{
	mObjectType = NAME_OBJECT_SCENE_COMPONENT;

	mOwnerActor           = InOwnerActor;
	mParentSceneComponent = InParentSceneComponent;
}

JSceneComponent::JSceneComponent(const JSceneComponent& Copy)
	: JActorComponent(Copy),
	  mParentSceneComponent(nullptr),
	  mWorldLocation(Copy.mWorldLocation),
	  mWorldRotation(Copy.mWorldRotation),
	  mWorldScale(Copy.mWorldScale),
	  mLocalLocation(Copy.mLocalLocation),
	  mLocalRotation(Copy.mLocalRotation),
	  mLocalScale(Copy.mLocalScale)
{
	for (int32_t i = 0; i < Copy.mChildSceneComponents.size(); ++i)
	{
		UPtr<JSceneComponent> newObj                   = MakeUPtr<JSceneComponent>(*Copy.mChildSceneComponents[i].get());
		newObj->mParentSceneComponent                  = this;
		mChildSceneComponentIndices[newObj->GetName()] = i;
		mChildSceneComponents.push_back(std::move(newObj));
	}
}


JSceneComponent::~JSceneComponent() {}

bool JSceneComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JActorComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	int32_t childCount = mChildSceneComponents.size();
	Utils::Serialization::Serialize_Primitive(&childCount, sizeof(int32_t), FileStream);


	for (int32_t i = 0; i < childCount; ++i)
	{
		JText childType = mChildSceneComponents[i]->GetObjectType();

		Utils::Serialization::Serialize_Text(childType, FileStream);

		mChildSceneComponents[i]->Serialize_Implement(FileStream);
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

	int32_t childCount;
	Utils::Serialization::DeSerialize_Primitive(&childCount, sizeof(int32_t), InFileStream);

	mChildSceneComponents.reserve(childCount);

	for (int32_t i = 0; i < childCount; ++i)
	{
		// 자식 씬 컴포넌트의 이름
		JText childType;
		Utils::Serialization::DeSerialize_Text(childType, InFileStream);

		UPtr<JSceneComponent> newObj  = UPtrCast<JSceneComponent>(MClassFactory::Get().Create(childType));
		newObj->mParentSceneComponent = this;
		newObj->SetOwnerActor(mOwnerActor);

		newObj->DeSerialize_Implement(InFileStream);
		mChildSceneComponentIndices[newObj->GetName()] = i;
		mChildSceneComponents.push_back(std::move(newObj));
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

	for (int32_t i = 0; i < mChildSceneComponents.size(); ++i)
	{
		mChildSceneComponents[i]->Tick(DeltaTime);
	}
}

void JSceneComponent::Draw()
{
	for (int32_t i = 0; i < mChildSceneComponents.size(); ++i)
	{
		mChildSceneComponents[i]->Draw();
	}
}

void JSceneComponent::DrawID(uint32_t ID)
{

	for (int32_t i = 0; i < mChildSceneComponents.size(); ++i)
	{
		mChildSceneComponents[i]->DrawID(ID);
	}
}

void JSceneComponent::SetWorldLocation(const FVector& InTranslation)
{
	mWorldLocation = InTranslation;
	UpdateWorldTransform();

	// 로컬 위치 계산
	if (mParentSceneComponent)
	{
		FMatrix parentWorldInverse = mParentSceneComponent->mWorldMat.Invert();
		mLocalMat                  = parentWorldInverse * mWorldMat;
		mLocalLocation             = FVector(mLocalMat.m[3][0], mLocalMat.m[3][1], mLocalMat.m[3][2]);
	}
	else
	{
		mLocalLocation = InTranslation;
	}
}

void JSceneComponent::SetWorldRotation(const FVector& InRotation)
{
	mWorldRotation = InRotation;
	UpdateWorldTransform();

	if (mParentSceneComponent)
	{
		// 월드 회전을 쿼터니언으로 변환
		DirectX::XMVECTOR worldQuat = DirectX::XMQuaternionRotationRollPitchYaw(
																				DirectX::XMConvertToRadians(InRotation.x),
																				DirectX::XMConvertToRadians(InRotation.y),
																				DirectX::XMConvertToRadians(InRotation.z));

		// 부모 월드 회전을 쿼터니언으로 변환
		DirectX::XMVECTOR parentQuat    = XMQuaternionRotationMatrix(mParentSceneComponent->mWorldMat);
		DirectX::XMVECTOR parentQuatInv = DirectX::XMQuaternionInverse(parentQuat);

		// 로컬 회전 = 부모 월드 회전의 역 * 월드 회전
		DirectX::XMVECTOR localQuat = DirectX::XMQuaternionMultiply(parentQuatInv, worldQuat);

		// 로컬 회전을 오일러 각도로 변환
		DirectX::XMVECTOR axis;
		float             angle;
		DirectX::XMQuaternionToAxisAngle(&axis, &angle, localQuat);

		mLocalRotation.x = axis.m128_f32[0] * angle;
		mLocalRotation.y = axis.m128_f32[1] * angle;
		mLocalRotation.z = axis.m128_f32[2] * angle;
	}
	else
	{
		mLocalRotation = InRotation;
	}
}

void JSceneComponent::SetWorldScale(const FVector& InScale)
{
	mWorldScale = InScale;
	UpdateWorldTransform();

	if (mParentSceneComponent)
	{
		FMatrix parentWorldInverse = mParentSceneComponent->mWorldMat.Invert();
		mLocalMat                  = parentWorldInverse * mWorldMat;

		// 각 축 벡터의 길이로 스케일 계산
		FVector scaleX(mLocalMat.m[0][0], mLocalMat.m[0][1], mLocalMat.m[0][2]);
		FVector scaleY(mLocalMat.m[1][0], mLocalMat.m[1][1], mLocalMat.m[1][2]);
		FVector scaleZ(mLocalMat.m[2][0], mLocalMat.m[2][1], mLocalMat.m[2][2]);

		mLocalScale = FVector(scaleX.Length(), scaleY.Length(), scaleZ.Length());
	}
	else
	{
		mLocalScale = InScale;
	}
}

void JSceneComponent::AddChildSceneComponent(JSceneComponent* Ptr)
{}

void JSceneComponent::SetupAttachment(JSceneComponent* InParentComponent)
{
	if (!InParentComponent || InParentComponent == mParentSceneComponent)
	{
		return;
	}

	const int32_t index = mOwnerActor->mChildSceneComponentIndices[GetName()];

	InParentComponent->mChildSceneComponentIndices[GetName()] = InParentComponent->mChildSceneComponents.size();
	InParentComponent->mChildSceneComponents.push_back(std::move(mOwnerActor->mChildSceneComponents[index]));

	// 원래 존재하던 mParentSceneComponent의 배열 업데이트
	mOwnerActor->mChildSceneComponentIndices.erase(GetName());
	mOwnerActor->mChildSceneComponents.erase(mOwnerActor->mChildSceneComponents.begin() + index);

	mParentSceneComponent = InParentComponent;
	SetOwnerActor(InParentComponent->GetOwnerActor());
}

void JSceneComponent::AttachComponent(JSceneComponent* InChildComponent)
{
	if (!InChildComponent)
	{
		return;
	}

}

void JSceneComponent::AttachToComponent(const Ptr<JSceneComponent>& InParentComponent)
{}

void JSceneComponent::AttachToActor(AActor* InParentActor, const JText& InComponentAttachTo)
{}

void JSceneComponent::AttachToActor(AActor* InParentActor, JSceneComponent* InComponentAttachTo)
{
	assert(InParentActor);


}

void JSceneComponent::DetachFromComponent()
{}

JSceneComponent* JSceneComponent::GetComponentByName(const JText& InName)
{
	auto it = mChildSceneComponentIndices.find(InName);
	if (it != mChildSceneComponentIndices.end())
	{
		return mChildSceneComponents[it->second].get();
	}
	return nullptr;
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
	if (mParentSceneComponent)
	{
		mWorldMat = mParentSceneComponent->mWorldMat * mLocalMat;
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

void JSceneComponent::UpdateWorldTransform()
{
	mWorldMat = DirectX::XMMatrixScaling(
										 mWorldScale.x,
										 mWorldScale.y,
										 mWorldScale.z) *
			DirectX::XMMatrixRotationRollPitchYaw(
												  DirectX::XMConvertToRadians(mWorldRotation.x),
												  DirectX::XMConvertToRadians(mWorldRotation.y),
												  DirectX::XMConvertToRadians(mWorldRotation.z)) *
			DirectX::XMMatrixTranslation(
										 mWorldLocation.x,
										 mWorldLocation.y,
										 mWorldLocation.z);
}

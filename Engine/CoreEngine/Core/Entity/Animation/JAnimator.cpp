#include "JAnimator.h"

#include "JAnimationClip.h"
#include "MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"

JAnimator::JAnimator() {}

JAnimator::JAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
	: mSkeletalMeshComponent(InSkeletalComp),
	  mCurrentAnimation(nullptr)
{}

JAnimator::JAnimator(const JAnimator& InCopy)
{
	for (auto& [key, value] : InCopy.mStateMachine)
	{
		mStateMachine[key] = GetWorld.AnimationManager->Clone(value->GetName());
	}
	mAnimLink                 = InCopy.mAnimLink;
	mSkeletalMeshComponent    = InCopy.mSkeletalMeshComponent;
	mSkeletalMeshInstanceData = InCopy.mSkeletalMeshInstanceData;
}

UPtr<IManagedInterface> JAnimator::Clone() const
{
	return MakeUPtr<JAnimator>(*this);
}

uint32_t JAnimator::GetHash() const
{
	return 0;
}

uint32_t JAnimator::GetType() const
{
	return HASH_ASSET_TYPE_ANIMATOR;
}

bool JAnimator::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// State Machine
	size_t stateMachineSize = mStateMachine.size();
	Utils::Serialization::Serialize_Primitive(&stateMachineSize, sizeof(stateMachineSize), FileStream);

	for (auto& [key, value] : mStateMachine)
	{
		JText keyStr       = key;
		JText animClipName = value->GetName();
		Utils::Serialization::Serialize_Text(keyStr, FileStream);
		Utils::Serialization::Serialize_Text(animClipName, FileStream);
	}

	return false;
}

bool JAnimator::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// State Machine
	size_t stateMachineSize;
	Utils::Serialization::DeSerialize_Primitive(&stateMachineSize, sizeof(stateMachineSize), InFileStream);

	mStateMachine.clear();
	mStateMachine.reserve(stateMachineSize);

	for (size_t i = 0; i < stateMachineSize; ++i)
	{
		JText keyStr;
		JText animClipName;
		Utils::Serialization::DeSerialize_Text(keyStr, InFileStream);
		Utils::Serialization::DeSerialize_Text(animClipName, InFileStream);


		JAnimationClip* animClip = GetWorld.AnimationManager->Load(animClipName);
		assert(animClip);
		mStateMachine[keyStr] = UPtrCast<JAnimationClip>(animClip->Clone());
	}
	return true;
}

void JAnimator::Initialize()
{
	if (!mSkeletalMeshComponent->GetOwnerActor())
	{
		return;
	}
	mMovementComponent = static_cast<JPawnMovementComponent*>(
		mSkeletalMeshComponent->GetOwnerActor()->
								GetChildComponentByType(NAME_COMPONENT_PAWN_MOVEMENT));
}

void JAnimator::BeginPlay()
{}

void JAnimator::Tick(float DeltaTime)
{
	if (mCurrentAnimation)
	{
		// mSkeletalMeshInstanceData = mCurrentAnimation->GetInstanceData();

		if (!mCurrentAnimation->GetNextState().empty())
		{
			SetState(mCurrentAnimation->GetNextState());
		}
	}
}

void JAnimator::Destroy()
{}

void JAnimator::AddAnimationClip(const JText& InState, const JAnimationClip* InClip)
{
	mStateMachine[InState] = UPtrCast<JAnimationClip>(InClip->Clone());
	mStateMachine[InState]->Initialize();
}

void JAnimator::AddAnimationClip(const JText& InState, const JText& InClipPath, const bool bEnableLoop)
{
	if (auto it = mStateMachine.find(InState); it != mStateMachine.end())
	{
		if (it->second->GetName() == InClipPath)
		{
			return;
		}
	}
	mStateMachine[InState] = GetWorld.AnimationManager->Clone(InClipPath, mSkeletalMeshComponent);
	mStateMachine[InState]->Initialize();
	mStateMachine[InState]->SetLoop(bEnableLoop);
}

void JAnimator::AddAnimLink(const JText& SrcState, const JText& DstState, const std::function<bool()>& InFunc,
							const float  InTransitionTime)
{
	auto srcIt = mStateMachine.find(SrcState);
	auto dstIt = mStateMachine.find(DstState);

	if (srcIt == mStateMachine.end() || dstIt == mStateMachine.end())
	{
		LOG_CORE_ERROR("Source{} -> Dest{} 둘 중 하나 이상의 상태가 존재하지 않음", SrcState, DstState);
		return;
	}

	srcIt->second->AddTransition(DstState, InFunc, InTransitionTime);
}

void JAnimator::SetState(const JText& InState)
{
	auto it = mStateMachine.find(InState);

	if (it == mStateMachine.end())
	{
		return;
	}

	mCurrentState     = InState;
	mCurrentAnimation = it->second.get();

	mSkeletalMeshComponent->SetAnimation(mCurrentAnimation);
}

float JAnimator::GetAnimElapsedRatio() const
{
	return mCurrentAnimation->GetElapsedRatio();
}

#include "JAnimator.h"

#include "JAnimationClip.h"
#include "MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Interface/JWorld.h"

JAnimator::JAnimator() {}

JAnimator::JAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
	: mSkeletalMeshComponent(InSkeletalComp),
	  mCurrentAnimation(nullptr)
{}

UPtr<IManagedInterface> JAnimator::Clone() const
{
	return {};
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
		mStateMachine[keyStr] = animClip;
	}
	return true;
}

void JAnimator::Initialize()
{}

void JAnimator::BeginPlay()
{}

void JAnimator::Tick(float DeltaTime)
{
	if (mCurrentAnimation)
	{
		mCurrentAnimation->TickAnim(DeltaTime);
	}
}

void JAnimator::Destroy()
{}

void JAnimator::AddAnimationClip(const JText& InState, JAnimationClip* InClip)
{
	mStateMachine[InState] = InClip;
}

void JAnimator::SetState(const JText& InState)
{
	auto it = mStateMachine.find(InState);

	if (it == mStateMachine.end())
	{
		return;
	}

	mPrevAnimation    = mCurrentAnimation;
	mCurrentState     = InState;
	mCurrentAnimation = it->second;

	mSkeletalMeshComponent->SetAnimation(mCurrentAnimation);
}

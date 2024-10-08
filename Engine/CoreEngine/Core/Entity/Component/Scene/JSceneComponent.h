#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/Math/TMatrix.h"

/**
 * 씬 컴포넌트는 씬에 배치되는 모든 액터의 기본 컴포넌트
 * 위치 및 회전, 크기를 가지며, 이를 통해 컴포넌트의 위치 및 회전을 결정
 */
class JSceneComponent : public JActorComponent, public IRenderable
{
public:
	JSceneComponent();
	JSceneComponent(JTextView InName);
	JSceneComponent(JTextView                   InName, const Ptr<JActor>& InOwnerActor,
					const Ptr<JSceneComponent>& InParentSceneComponent);
	~JSceneComponent() override;

public:
	EObjectType GetObjectType() const override { return EObjectType::SceneComponent; }
	uint32_t    GetType() const override { return StringHash("JSceneComponent"); }

public:
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Tick(float DeltaTime) override;

public:
	void PreRender() override {}
	void Render() override {};
	void PostRender() override {};
	void Draw() override;

public:
	FORCEINLINE [[nodiscard]] FVector GetWorldLocation() const { return mWorldLocation; }
	FORCEINLINE [[nodiscard]] FVector GetWorldRotation() const { return mWorldRotation; }
	FORCEINLINE [[nodiscard]] FVector GetWorldScale() const { return mWorldScale; }

	void SetWorldLocation(const FVector& InTranslation) { mWorldLocation = InTranslation; }
	void SetWorldRotation(const FVector& InRotation) { mWorldRotation = InRotation; }
	void SetWorldScale(const FVector& InScale) { mWorldScale = InScale; }

	FORCEINLINE [[nodiscard]] FVector GetLocalLocation() const { return mLocalLocation; }
	FORCEINLINE [[nodiscard]] FVector GetLocalRotation() const { return mLocalRotation; }
	FORCEINLINE [[nodiscard]] FVector GetLocalScale() const { return mLocalScale; }

	void SetLocalLocation(const FVector& InTranslation) { mLocalLocation = InTranslation; }
	void SetLocalRotation(const FVector& InRotation) { mLocalRotation = InRotation; }
	void SetLocalScale(const FVector& InScale) { mLocalScale = InScale; }

	void SetParentSceneComponent(const Ptr<JSceneComponent>& Ptr) { mParentSceneComponent = Ptr; }
	void AddChildSceneComponent(const Ptr<JSceneComponent>& Ptr);

	/**
	 *	씬 컴포넌트를 부모 컴포넌트에 부착
	 * @param InParentComponent 
	 */
	void SetupAttachment(const Ptr<JSceneComponent>& InParentComponent);

	void AttachComponent(const Ptr<JSceneComponent>& InChildComponent);

	/**
	 * 씬 컴포넌트를 다른 씬 컴포넌트에 부착
	 * @param InParentComponent 부착 될(부모) 씬 컴포넌트
	 */
	void AttachToComponent(const Ptr<JSceneComponent>& InParentComponent);

	void AttachToActor(const Ptr<JActor>& InParentActor, const JText& InComponentAttachTo = "RootComponent");
	void AttachToActor(const Ptr<JActor>& InParentActor, const Ptr<JSceneComponent>& InComponentAttachTo);

	void DetachFromComponent();

public:
	void UpdateTransform();

protected:
	// ----------------------------- Scene Component Data -----------------------------
	WPtr<JSceneComponent>         mParentSceneComponent;
	JArray<WPtr<JSceneComponent>> mChildSceneComponents;

	// ----------------------------- World Transform Data -----------------------------
	FVector mWorldLocation = FVector::ZeroVector;
	FVector mWorldRotation = FVector::ZeroVector;
	FVector mWorldScale    = FVector::OneVector;

	FMatrix mWorldLocationMat = FMatrix::Identity;
	FMatrix mWorldRotationMat = FMatrix::Identity;
	FMatrix mWorldScaleMat    = FMatrix::Identity;
	FMatrix mWorldMat         = FMatrix::Identity;

	// ----------------------------- Local Transform Data -----------------------------
	FVector mLocalLocation = FVector::ZeroVector;
	FVector mLocalRotation = FVector::ZeroVector;
	FVector mLocalScale    = FVector::OneVector;

	FMatrix mLocalLocationMat = FMatrix::Identity;
	FMatrix mLocalRotationMat = FMatrix::Identity;
	FMatrix mLocalScaleMat    = FMatrix::Identity;
	FMatrix mLocalMat         = FMatrix::Identity;
};

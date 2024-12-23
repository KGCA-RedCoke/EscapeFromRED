﻿#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/Math/TMatrix.h"
#include "Shape/JShape.h"

/**
 * 씬 컴포넌트는 씬에 배치되는 모든 액터의 기본 컴포넌트
 * 위치 및 회전, 크기를 가지며, 이를 통해 컴포넌트의 위치 및 회전을 결정
 */
class JSceneComponent : public JActorComponent, public IRenderable
{
public:
	JSceneComponent();
	JSceneComponent(JTextView        InName,
					AActor*          InOwnerActor           = nullptr,
					JSceneComponent* InParentSceneComponent = nullptr);
	JSceneComponent(const JSceneComponent& Copy);
	~JSceneComponent() override;

public:
	uint32_t GetType() const override { return StringHash("JSceneComponent"); }

public:
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Tick(float DeltaTime) override;

public:
	void PreRender() override {}
	void AddInstance(float InCameraDistance) override {};
	void PostRender() override {};
	void Draw() override;
	void DrawID(uint32_t ID) override;

public:
	void ShowEditor() override;

public:
	FORCEINLINE [[nodiscard]] FMatrix GetWorldMatrix() const { return mWorldMat; }
	FORCEINLINE [[nodiscard]] FVector GetWorldLocation() const { return mWorldLocation; }
	FORCEINLINE [[nodiscard]] FVector GetWorldRotation() const { return mWorldRotation; }
	FORCEINLINE [[nodiscard]] FVector GetWorldScale() const { return mWorldScale; }

	FORCEINLINE [[nodiscard]] FVector GetLocalLocation() const { return mLocalLocation; }
	FORCEINLINE [[nodiscard]] FVector GetLocalRotation() const { return mLocalRotation; }
	FORCEINLINE [[nodiscard]] FVector GetLocalScale() const { return mLocalScale; }

	FORCEINLINE [[nodiscard]] FBoxShape GetBoundingVolume() const { return mBoundingBox; }

	void SetWorldLocation(const FVector& InTranslation);
	void SetWorldRotation(const FVector& InRotation);
	void SetWorldScale(const FVector& InScale);

	void SetLocalLocation(const FVector& InTranslation);
	void SetLocalRotation(const FVector& InRotation);
	void SetLocalScale(const FVector& InScale);

	void SetParentSceneComponent(JSceneComponent* Ptr) { mParentSceneComponent = Ptr; }
	void AddChildSceneComponent(JSceneComponent* Ptr);

	/**
	 *	씬 컴포넌트를 부모 컴포넌트에 부착
	 * @param InParentComponent 
	 */
	void SetupAttachment(JSceneComponent* InParentComponent);

	void AttachComponent(JSceneComponent* InChildComponent);

	/**
	 * 씬 컴포넌트를 다른 씬 컴포넌트에 부착
	 * @param InParentComponent 부착 될(부모) 씬 컴포넌트
	 */
	void AttachToComponent(const Ptr<JSceneComponent>& InParentComponent);

	void AttachToActor(AActor* InParentActor, const JText& InComponentAttachTo = "RootComponent");
	void AttachToActor(AActor* InParentActor, JSceneComponent* InComponentAttachTo);

	/**
	 * 스켈레톤 구조체를 가지는 컴포넌트에 이 컴포넌트를 부착
	 * @param InParent 부착될 스켈레탈 메시
	 * @param InSocketName 부착될 본 이름
	 */
	void AttachToBoneSocket(class JSkeletalMeshComponent* InParent, JTextView InSocketName);

	void DetachFromComponent();

	JSceneComponent* GetComponentByName(const JText& InName);

	int32_t GetChildCount() const { return mChildSceneComponents.size(); }

public:
	virtual void UpdateTransform();
	void         UpdateWorldTransform();

protected:
	// ----------------------------- Hierarchical Data -----------------------------
	JSceneComponent*              mParentSceneComponent;
	JHash<JText, int32_t>         mChildSceneComponentIndices;
	JArray<UPtr<JSceneComponent>> mChildSceneComponents;

	// ----------------------------- Bounding Box Data -----------------------------
	FBoxShape mBoundingBox;

	// ----------------------------- LevelManager Transform Data -----------------------------
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

	FMatrix mCachedLocalMat  = FMatrix::Identity;
	FMatrix mCachedParentMat = FMatrix::Identity;

	JSkeletalMeshComponent* mParentSkeletal;
	JText                   mSocketName;

	friend class GUI_Editor_Actor;
	friend class GUI_Inspector;
};

REGISTER_CLASS_TYPE(JSceneComponent);


// class JShape : public JSceneComponent
// {
// public:
// 	virtual void GenCollisionData() = 0;
// };

class JBoxComponent : public JSceneComponent, public IRenderable
{
public:
	JBoxComponent();
	JBoxComponent(JTextView InName, AActor* InOwnerActor = nullptr, JSceneComponent* InParentSceneComponent = nullptr);
	~JBoxComponent() override = default;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;

public:
	void PreRender() override {}
	void AddInstance(float InCameraDistance) override {};
	void PostRender() override {};
	void Draw() override;
	void DrawID(uint32_t ID) override {};

public:
	void ShowEditor() override;
};

REGISTER_CLASS_TYPE(JBoxComponent);

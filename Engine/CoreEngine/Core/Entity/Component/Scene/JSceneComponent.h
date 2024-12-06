#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "Core/Interface/IGUIEditable.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/Math/TMatrix.h"
#include "Shape/JShape.h"

class JMaterialInstance;
/**
 * 2D 스크린에 배치되는 컴포넌트
 */
class JSceneComponent_2D : public JActorComponent, public IRenderable
{
public:
	JSceneComponent_2D();
	JSceneComponent_2D(JTextView InName, AActor* InOwnerActor = nullptr);
	~JSceneComponent_2D() override;

public:
	uint32_t GetType() const override { return StringHash("JSceneComponent_2D"); }
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;

public:
	void PreRender() override {}
	void AddInstance(float InCameraDistance) override {};
	void PostRender() override {};
	void Draw() override {};
	void DrawID(uint32_t ID) override {};

public:
	FORCEINLINE [[nodiscard]] FVector2 GetLocalPosition() const { return mLocalPosition; }
	FORCEINLINE [[nodiscard]] FVector2 GetLocalRotation() const { return mLocalRotation; }
	FORCEINLINE [[nodiscard]] FVector2 GetLocalSize() const { return mLocalSize; }

	FORCEINLINE [[nodiscard]] FVector2 GetWorldPosition() const { return mWorldPosition; }
	FORCEINLINE [[nodiscard]] FVector2 GetWorldRotation() const { return mWorldRotation; }
	FORCEINLINE [[nodiscard]] FVector2 GetWorldSize() const { return mWorldSize; }

	void SetLocalPosition(const FVector2& InPosition) { mLocalPosition = InPosition; }
	void SetLocalRotation(const FVector2& InRotation) { mLocalRotation = InRotation; }
	void SetLocalSize(const FVector2& InSize) { mLocalSize = InSize; }

	void SetWorldPosition(const FVector2& InPosition) { mWorldPosition = InPosition; }
	void SetWorldRotation(const FVector2& InRotation) { mWorldRotation = InRotation; }
	void SetWorldSize(const FVector2& InSize) { mWorldSize = InSize; }

protected:
	FVector2 mLocalPosition = FVector2::ZeroVector;
	FVector2 mLocalRotation = FVector2::ZeroVector;
	FVector2 mLocalSize     = FVector2::UnitVector;

	FVector2 mWorldPosition = FVector2::ZeroVector;
	FVector2 mWorldRotation = FVector2::ZeroVector;
	FVector2 mWorldSize     = FVector2::UnitVector;

	FMatrix mWorldMat = FMatrix::Identity;
	FMatrix mLocalMat = FMatrix::Identity;

	JMaterialInstance* m2DMaterialInstance = nullptr;
};


/**
 * 씬 컴포넌트는 씬에 배치되는 모든 액터의 기본 컴포넌트
 * 위치 및 회전, 크기를 가지며, 이를 통해 컴포넌트의 위치 및 회전을 결정
 */
class JSceneComponent : public JActorComponent, public IRenderable, public IGUIEditable
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

	void SetLocalLocation(const FVector& InTranslation) { mLocalLocation = InTranslation; }
	void SetLocalRotation(const FVector& InRotation) { mLocalRotation = InRotation; }
	void SetLocalScale(const FVector& InScale) { mLocalScale = InScale; }

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

	void DetachFromComponent();

	JSceneComponent* GetComponentByName(const JText& InName);

	int32_t GetChildCount() const { return mChildSceneComponents.size(); }

public:
	void UpdateTransform();
	void UpdateWorldTransform();

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

	FMatrix mCachedLocalMat = FMatrix::Identity;

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


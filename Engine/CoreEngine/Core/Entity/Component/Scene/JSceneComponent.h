#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/Math/TMatrix.h"
#include "Shape/JShape.h"
#include "Shape/Collision/MCollisionManager.h"

namespace Quad
{
	struct FNode;
}
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
	void Initialize() override;
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

	void AddLocalLocation(const FVector& InTranslation);
	void SetLocalLocation(const FVector& InTranslation);
	void SetLocalRotation(const FVector& InRotation);
	void SetLocalScale(const FVector& InScale);

	void SetParentSceneComponent(JSceneComponent* Ptr) { mParentSceneComponent = Ptr; }
	JSceneComponent* GetParentSceneComponent(){ return mParentSceneComponent; }
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

	bool    bLocalDirty     = false;
	FMatrix mCachedWorldMat = FMatrix::Identity;

	JSkeletalMeshComponent* mParentSkeletal;
	JText                   mSocketName;

	friend class GUI_Editor_Actor;
	friend class GUI_Inspector;
};


class JCollisionComponent : public JSceneComponent, public ICollision
{
public:
	JCollisionComponent() = default;

	JCollisionComponent(JTextView InName, AActor* InOwner = nullptr, JSceneComponent* InParent = nullptr)
		: JSceneComponent(InName, InOwner, InParent) {};
	~JCollisionComponent() override = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize() override;
	void BeginPlay() override;
	void Destroy() override;

public:
	FBoxShape      GetBox() const override;
	FRay           GetRay() const override;
	FSphere        GetSphere() const override;
	uint32_t       GetCollisionID() override;
	uint32_t       GetActorID() const override;
	ETraceType     GetTraceType() const override { return mTraceType; }
	ECollisionType GetCollisionType() const override { return mCollisionType; }
	void           SetTraceType(ETraceType InType) override { mTraceType = InType; }
	void           SetCollisionType(ECollisionType InType) override { mCollisionType = InType; }
	bool           Intersect(ICollision* InOther, FHitResult& OutHitResult) const override;
	Quad::FNode*   GetDivisionNode() const;

public:
	void ShowEditor() override;
	void SetColor(const DirectX::XMVECTOR& InColor) { mColor = InColor; }

protected:
	ETraceType        mTraceType     = ETraceType::Pawn;
	ECollisionType    mCollisionType = ECollisionType::None;
	DirectX::XMVECTOR mColor{{0.678431392f, 1.f, 0.184313729f, 1.f}};
};

class JLineComponent : public JCollisionComponent
{
public:
	JLineComponent();
	JLineComponent(JTextView InName, AActor* InOwnerActor = nullptr, JSceneComponent* InParentSceneComponent = nullptr);
	~JLineComponent() override = default;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Draw() override;

public:
	void ShowEditor() override;

public:
	bool           Intersect(ICollision* InOther, FHitResult& OutHitResult) const override;
	ECollisionType GetCollisionType() const override;
	FRay           GetRay() const override;

protected:
	FRay mRay;
};

namespace CollisionEvent
{
	constexpr const char* OnEnter[] =
	{
		"Block",
		"Player Damage"
	};

	constexpr const char* OnOverlap[] =
	{
		"Block",
		"Player Damage"
	};

	constexpr const char* OnExit[] =
	{
		"Block",
		"Player Damage"
	};

}

class JBoxComponent : public JCollisionComponent
{
public:
	JBoxComponent();
	JBoxComponent(JTextView InName, AActor* InOwnerActor = nullptr, JSceneComponent* InParentSceneComponent = nullptr);
	~JBoxComponent() override = default;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Draw() override;

public:
	void ShowEditor() override;

public:
	bool           Intersect(ICollision* InOther, FHitResult& OutHitResult) const override;
	ECollisionType GetCollisionType() const override;
	FBoxShape      GetBox() const override;
};

REGISTER_CLASS_TYPE(JSceneComponent);
REGISTER_CLASS_TYPE(JLineComponent);
REGISTER_CLASS_TYPE(JBoxComponent);

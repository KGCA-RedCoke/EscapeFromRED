#pragma once
#include "GUI/Editor/GUI_Editor_Base.h"

class JActorComponent;

enum EComponentType : uint8_t
{
	Transform,
	Mesh,
	Camera,
	PointLight,
	SpotLight,
	BoxComponent,
	LineComponent,
	SphereComponent,
	CapsuleCollider,
	SpawnerComponent,
	CharacterController,
	NavMesh,
	Path,
	Waypoint,
	Trigger,
	Sound,
	Particle,
	Decal,
	Billboard,
	Widget,
	Script,
	Custom
};

constexpr const char* g_ComponentList[] = {
	"Transform",
	"Mesh",
	"Camera",
	"PointLight",
	"SpotLight",
	"BoxComponent",
	"LineComponent",
	"SphereComponent",
	"CapsuleComponent",
	"SpawnerComponent",
	"Character Controller",
	"NavMesh",
	"Path",
	"Waypoint",
	"Trigger",
	"Sound",
	"Particle",
	"Decal",
	"Billboard",
	"Widget",
	"Script",
	"Custom"
};


class GUI_Editor_Actor : public GUI_Editor_Base
{
public:
	/** 생성자 인자로 액터경로(이미 존재하는 에셋일 경우) 또는 액터 이름(새로 만들경우)를 넘겨준다. */
	GUI_Editor_Actor(const JText& InPath, JText InClassName = "AActor");

	~GUI_Editor_Actor() override = default;

public:
	void ShowMenuBar() override;

public:
	void Initialize() override;
	void Update_Implementation(float DeltaTime) override;
	void Render() override;

private:
	/**
	 * 창 왼쪽 트리 뷰에 표시할 액터
	 */
	void DrawHierarchy();


	void DrawTreeNode(class JSceneComponent* InSceneComponent);

	void OnDragDropTarget();

	/**
	 * 액터의 렌더 정보를 표시할 뷰포트
	 */
	void DrawViewport() const;

	/**
	 * 내부의 컴포넌트 정보를 표시할 디테일 뷰
	 */
	void DrawDetails();

private:
	UPtr<class AActor> mActorToEdit;			// 편집할 액터
	JActorComponent*   mSelectedSceneComponent;
	bool               bAddComponentListBox = false;
	JText              mClassName;
};

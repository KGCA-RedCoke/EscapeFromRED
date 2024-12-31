#include "Widget_IntroScene.h"

Widget_IntroScene::Widget_IntroScene(const JText& InName)
	: JWidgetComponent(InName)
{
	Utils::Serialization::DeSerialize("Game/UI/IntroScene.jasset", this);

	mPressAnyKey = mUIComponents[0].get();
}

void Widget_IntroScene::Initialize()
{
	JWidgetComponent::Initialize();
}

void Widget_IntroScene::BeginPlay()
{
	JWidgetComponent::BeginPlay();
}

void Widget_IntroScene::Tick(float DeltaTime)
{
	JWidgetComponent::Tick(DeltaTime);
}

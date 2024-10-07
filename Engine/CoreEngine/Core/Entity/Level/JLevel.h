#pragma once
#include "Core/Entity/JObject.h"

class JActor;

/**
 * 레벨에서는 액터 관리(환경, 배치)를 담당한다.
 */
class JLevel : public ISerializable
{
public:
	JLevel(const JText& InName);
	~JLevel() override;
	

	uint32_t GetType() const override;

	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;


	void AddActor(JActor* InActor);
	void LoadActorFromPath(const JText& InPath);

	JActor* CreateActor(const JText& InName);

private:
	JArray<JActor*> mActors;

};

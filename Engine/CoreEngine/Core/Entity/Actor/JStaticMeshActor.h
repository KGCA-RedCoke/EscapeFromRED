#pragma once
#include "JActor.h"

class JMeshObject;

class JStaticMeshActor : public JActor
{
public:
	/**
	 * 단순한 메시 오브젝트로부터 액터 생성
	 * @param InName 오브젝트 고유 네임
	 * @param InMeshObject 메시 오브젝트 (Manager에서 생성된 메시 오브젝트(버퍼를 가지고 있는)) 
	 */
	JStaticMeshActor(JTextView InName, const Ptr<JMeshObject>& InMeshObject);

	/**
	 * 단순한 메시 오브젝트로부터 액터 생성
	 * @param InName 오브젝트 고유 네임
	 * @param SavedMeshPath 저장된 메시 경로(jasset) 
	 */
	JStaticMeshActor(JTextView InName, JTextView SavedMeshPath);

public:
	void Initialize() override;

private:
	void CreateMeshComponent(const Ptr<JMeshObject>& InMeshObject);

private:
	Ptr<JMeshObject> mMeshObject = nullptr;
};

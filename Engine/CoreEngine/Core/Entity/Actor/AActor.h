#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Entity/Component/Scene/JSceneComponent.h"


class JActorComponent;
class JTransformComponent;

class AActor : public JSceneComponent
{
public:
	AActor();
	AActor(JTextView InName);
	AActor(const AActor& Copy);
	~AActor() override = default;

	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

	void Draw() override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	JSceneComponent* GetChildSceneComponentByName(JTextView InName) const;
	JSceneComponent* GetChildSceneComponentByType(JTextView InType) const;
	JActorComponent* GetChildComponentByType(JTextView InType) const;

	template <class ObjectType, typename... Args>
	ObjectType* CreateDefaultSubObject(Args... args)
	{
		static_assert(std::is_base_of_v<JObject, ObjectType>, "ObjectType is not derived from JObject");

		UPtr<ObjectType> obj    = MakeUPtr<ObjectType>(std::forward<Args>(args)...);
		ObjectType*      objPtr = obj.get();
		obj->SetOwnerActor(this);

		if constexpr (std::is_base_of_v<JSceneComponent, ObjectType>)
		{
			if (mChildSceneComponentIndices.contains(obj->GetName()))
			{
				const int32_t index = mChildSceneComponentIndices[obj->GetName()];
				objPtr              = static_cast<ObjectType*>(mChildSceneComponents.at(index).get());
				return objPtr;
			}

			mChildSceneComponentIndices.insert({obj->GetName(), mChildSceneComponents.size()});
			mChildSceneComponents.push_back(std::move(obj));
		}

		else if constexpr (std::is_base_of_v<JActorComponent, ObjectType>)
		{
			// if (mChildActorIndices.contains(obj->GetName()))
			// {
			// 	const int32_t index = mChildActorIndices[obj->GetName()];
			// 	objPtr              = mChildActors.at(index);
			// 	return static_cast<ObjectType*>(objPtr);
			// }

			// mChildActorIndices.insert({obj->GetName(), mChildActors.size()});
			mActorComponents.push_back(std::move(obj));
		}

		return objPtr;
	}

protected:
	AActor*         mParentActor;
	JArray<AActor*> mChildActors;

	JHash<JText, int32_t>         mChildActorIndices;
	JArray<UPtr<JActorComponent>> mActorComponents;

};

REGISTER_CLASS_TYPE(AActor);

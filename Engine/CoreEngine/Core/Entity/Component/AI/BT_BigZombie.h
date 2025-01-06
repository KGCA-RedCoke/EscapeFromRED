#pragma once
// #include "Core/Input/XKeyboardMouse.h"
#include "BtBase.h"

class AEnemy;
class AStar;

class BT_BigZombie : public BtBase,
                public std::enable_shared_from_this<BT_BigZombie>
{
public:
    BT_BigZombie(JTextView InName, AActor* InOwner);
    ~BT_BigZombie() override;

    void Initialize() override;
    void BeginPlay() override;
    void Destroy() override;

    void Tick(float DeltaTime) override;
    void SetupTree();
    void SetupTree2();
    void ResetBT() override;
public:
    // Action Function
    NodeStatus Attack();
    NodeStatus Attack2();
    // NodeStatus JumpAttack();
    // void       MoveNPCWithJump(float jumpHeight, float duration);
    NodeStatus Hit();
    NodeStatus Dead();

    // Decorator Function


    // Just Function


public:
    AEnemy* mOwnerEnemy;
///ㅁㄴㅇㄻㄴㅇㄹ
private:
    // FORCEINLINE bool IsKeyPressed(EKeyCode InKey) const { return mInputKeyboard.IsKeyPressed(InKey); }
    // FORCEINLINE bool IsKeyDown(EKeyCode InKey) const { return mInputKeyboard.IsKeyDown(InKey); }
    // XKeyboardMouse mInputKeyboard;
};

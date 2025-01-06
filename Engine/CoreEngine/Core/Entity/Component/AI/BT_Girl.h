#pragma once
#include "Core/Input/XKeyboardMouse.h"
#include "BtBase.h"

class AEnemy;
class AStar;

class BT_Girl : public BtBase,
                public std::enable_shared_from_this<BT_Girl>
{
public :
    BT_Girl(JTextView InName, AActor* InOwner);
    ~BT_Girl() override;

    void Initialize() override;
    void BeginPlay() override;
    void Destroy() override;

    void Tick(float DeltaTime) override;
    void SetupTree();
    void ResetBT(AActor* NewOwner) override;

public:
    // Action Function
    NodeStatus Attack();
    NodeStatus conversation(int idx);
    // NodeStatus JumpAttack();
    // void       MoveNPCWithJump(float jumpHeight, float duration);
    NodeStatus IsPressedKey(EKeyCode Key);
    NodeStatus Hit();
    NodeStatus Dead();

    // Decorator Function


    // Just Function


public:
    AEnemy* mOwnerEnemy;

private:
    FORCEINLINE bool IsKeyPressed(EKeyCode InKey) const { return mInputKeyboard.IsKeyPressed(InKey); }
    FORCEINLINE bool IsKeyDown(EKeyCode InKey) const { return mInputKeyboard.IsKeyDown(InKey); }
    XKeyboardMouse mInputKeyboard;
};

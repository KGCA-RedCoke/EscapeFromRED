#include "ACharacter.h"

#include "Core/Input/XKeyboardMouse.h"

ACharacter::ACharacter() {}
ACharacter::ACharacter(JTextView InName) {}

void SampleCharacter::SetupInputComponent()
{
	// mInput->AddInputBinding(EKeyCode::A,
	// 						EKeyState::Down,
	// 						std::bind(&SampleCharacter::MoveLeft, this, std::placeholders::_1));
}

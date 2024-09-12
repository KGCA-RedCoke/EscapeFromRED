#pragma once

enum class EKeyCode : uint8_t
{
	Q,
	W,
	E,
	R,
	T,
	Y,
	U,
	I,
	O,
	P,
	A,
	S,
	D,
	F,
	G,
	H,
	J,
	K,
	L,
	Z,
	X,
	C,
	V,
	B,
	N,
	M,
	Left,
	Right,
	Down,
	Up,
	LButton,
	MButton,
	RButton,
	Space,
	F1,
	F2,
	F3,
	F4,
	LShift,
	End
};

enum class EKeyState : uint8_t
{
	None,    // 기본
	Up,      // 떼어짐
	Down,    // 눌림 (입력 소모)
	Pressed, // 눌린 상태
};


static constexpr int ASCII[static_cast<UINT>(EKeyCode::End)] =
{
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	VK_LEFT, VK_RIGHT, VK_DOWN, VK_UP,
	VK_LBUTTON, VK_MBUTTON, VK_RBUTTON,
	VK_SPACE, VK_F1, VK_F2, VK_F3, VK_F4,
	VK_LSHIFT
};

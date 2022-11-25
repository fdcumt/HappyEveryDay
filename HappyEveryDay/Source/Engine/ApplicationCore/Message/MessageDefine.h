#pragma once
#include "HAL/Platform.h"

namespace NS_Message
{
	enum EInputEvent
	{
		IE_Pressed = 0,
		IE_Released = 1,
		IE_Repeat = 2,
		IE_DoubleClick = 3,
		IE_Axis = 4,
		IE_MAX = 5,
	};

	enum EMesssageType
	{
		E_MessageType_None = 0,

		E_MouseButton_Left,
		E_MouseButton_Right,

		E_Keyboard_A,
		E_Keyboard_B,
		E_Keyboard_C,
		E_Keyboard_D,
		E_Keyboard_E,
		E_Keyboard_F,
		E_Keyboard_G,
		E_Keyboard_H,
		E_Keyboard_I,
		E_Keyboard_J,
		E_Keyboard_K,
		E_Keyboard_L,
		E_Keyboard_M,
		E_Keyboard_N,
		E_Keyboard_O,
		E_Keyboard_P,
		E_Keyboard_Q,
		E_Keyboard_R,
		E_Keyboard_S,
		E_Keyboard_T,
		E_Keyboard_U,
		E_Keyboard_V,
		E_Keyboard_W,
		E_Keyboard_X,
		E_Keyboard_Y,
		E_Keyboard_Z,

		E_MesssageType_Max
	};

}



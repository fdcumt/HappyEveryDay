#pragma once
#include "Message/MessageDefine.h"

namespace NS_Message
{
	struct FMessageInfo
	{
		EInputEvent InputEvent = EInputEvent::IE_Released;
	};

	extern FMessageInfo MessageInfoList[EMesssageType::E_MesssageType_Max + 1];

}


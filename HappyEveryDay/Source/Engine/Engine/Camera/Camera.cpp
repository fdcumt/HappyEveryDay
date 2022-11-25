#include "Camera.h"
#include <cmath>
#include "Logging/LogMarcos.h"
#include "Logging/Log.h"

DEFINE_LOG_CATEGORY(CameraLog);


FCamera::FCamera()
	: FOV(45.f)
	, NearPlane(0.1f)
	, FarPlane(1000.f)
	, Pitch(0.f)
	, Yaw(0.f)
	, Position(0.0f, 0.0f, 0.0f)
	, Up(0.f, 1.f, 0.f)
{

}


void FCamera::Init(const glm::vec3& InPosition, const float InPitch, const float InYaw, const float InFov, const float InNeerPlane, const float InFarPlane)
{
	Position = InPosition;

	Pitch = InPitch;
	Yaw = InYaw;

	FOV = InFov;

	NearPlane = InNeerPlane;
	FarPlane = InFarPlane;

	bDirty = true;
	RecalculateViewMatrix();
}

void FCamera::AddFov(const float InDeltaFov)
{
	FOV += InDeltaFov;
	FOV = 
	bDirty = true;
}

void FCamera::SetYaw(float InYaw)
{
	Yaw = InYaw;
	bDirty = true;
}

void FCamera::AddYaw(float InYaw)
{
	Yaw += InYaw;
	bDirty = true;
}

void FCamera::SetPitch(float InPitch)
{
	Pitch = InPitch;
	bDirty = true;
}

void FCamera::AddPitch(float InPitch)
{
	Pitch += InPitch;
	bDirty = true;
}

void FCamera::AddPositionOffset(const glm::vec3& InPosition)
{
	Position += InPosition;
	bDirty = true;
}

void FCamera::SetPosition(const glm::vec3& InPosition)
{
	Position = InPosition;
	bDirty = true;
}

void FCamera::DoMovement(NS_Message::FMessageInfo* MessageInfoList, float InMouseXDeltaMove, float InMouseYDeltaMove)
{
	if (MessageInfoList[NS_Message::E_MouseButton_Left].InputEvent==NS_Message::IE_Pressed)
	{
		{ // camera move
			float MoveSpeed = 0.1;
			if (MessageInfoList[NS_Message::E_Keyboard_W].InputEvent != NS_Message::IE_Released)
			{
				ForwardMoveDelta += MoveSpeed;
			}

			if (MessageInfoList[NS_Message::E_Keyboard_S].InputEvent != NS_Message::IE_Released)
			{
				ForwardMoveDelta -= MoveSpeed;
			}

			if (MessageInfoList[NS_Message::E_Keyboard_A].InputEvent != NS_Message::IE_Released)
			{
				RightMoveDelta -= MoveSpeed;
			}

			if (MessageInfoList[NS_Message::E_Keyboard_D].InputEvent != NS_Message::IE_Released)
			{
				RightMoveDelta += MoveSpeed;
			}

			if (ForwardMoveDelta != 0.f || RightMoveDelta != 0.f)
			{
				bDirty = true;
			}
		}

		{ // camera rotation
			float MoveSpeed = 0.1;
			Yaw += InMouseXDeltaMove * MoveSpeed;
			Pitch += InMouseYDeltaMove * MoveSpeed;

			if (InMouseXDeltaMove != 0.f || InMouseYDeltaMove != 0.f)
			{
				bDirty = true;
			}
		}
	}
}

void FCamera::RecalculateViewMatrix()
{
	if (bDirty)
	{
		bDirty = false;
		Forward.x = std::cos(glm::radians(Pitch)) * std::sin(glm::radians(Yaw));
		Forward.z = -std::cos(glm::radians(Pitch)) * std::cos(glm::radians(Yaw));
		Forward.y = std::sin(glm::radians(Pitch));

		glm::vec3 Right = glm::cross(Forward, Up);

		Position += Forward*ForwardMoveDelta;
		Position += Right*RightMoveDelta;


		ForwardMoveDelta = 0.f;
		RightMoveDelta = 0.f;

		ViewMatrix = glm::lookAt(Position, Position + Forward, Up);
	}
}

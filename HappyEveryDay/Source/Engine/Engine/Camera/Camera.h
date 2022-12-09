#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Message/MessageBase.h"



class FCamera
{
public:
	FCamera();

public:
	void Init(const glm::vec3& InPosition, const float InPitch, const float InYaw, const float InFov, const float InNeerPlane, const float InFarPlane);

public:
	const glm::mat4& GetViewMatrix() const { return ViewMatrix; }

	const float GetFov() const { return FOV; }
	void AddFov(const float InDeltaFov);
	void SetFov(const float InFov) { FOV = InFov; }

	const float GetNearPlane() const { return NearPlane; }
	void SetNearPlane(float InNearPlane) { NearPlane = InNearPlane; }

	const float GetFarPlane() const { return FarPlane; }
	void SetFarPlane(float InFarPlane) { FarPlane = InFarPlane; }

	void SetYaw(float InYaw);
	void AddYaw(float InYaw);

	void SetPitch(float InPitch);
	void AddPitch(float InPitch);

	void AddPositionOffset(const glm::vec3& InPosition);
	void SetPosition(const glm::vec3& InPosition);
	glm::vec3 GetPosition() const { return Position; }

	void DoMovement(NS_Message::FMessageInfo *MessageInfoList, float InMouseXDeltaMove, float InMouseYDeltaMove);

	void RecalculateViewMatrix();

protected:
	bool bDirty = false;

	float FOV = 45.f;

	float NearPlane = 1.f;
	float FarPlane = 1000.f;

	float Pitch = 0.f; 
	float Yaw = 0.f; // zox平面, 以z方向为初始朝向, 顺时针转动

	glm::vec3 Position;
	glm::vec3 Up;
	glm::vec3 Forward;
	glm::mat4 ViewMatrix;

	float ForwardMoveDelta = 0.f;
	float RightMoveDelta = 0.f;
};

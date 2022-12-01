#pragma once

class FVector
{
public:
	float X;
	float Y;
	float Z;

public:
	/** A zero vector (0,0,0) */
	static const FVector ZeroVector;

	/** One vector (1,1,1) */
	static const FVector OneVector;

	/** Unreal up vector (0,0,1) */
	static const FVector UpVector;

	/** Unreal down vector (0,0,-1) */
	static const FVector DownVector;

	/** Unreal forward vector (1,0,0) */
	static const FVector ForwardVector;

	/** Unreal backward vector (-1,0,0) */
	static const FVector BackwardVector;

	/** Unreal right vector (0,1,0) */
	static const FVector RightVector;

	/** Unreal left vector (0,-1,0) */
	static const FVector LeftVector;

public:
	FVector(float InX, float InY, float InZ)
		: X(InX), Y(InY), Z(InZ)
	{

	}


};

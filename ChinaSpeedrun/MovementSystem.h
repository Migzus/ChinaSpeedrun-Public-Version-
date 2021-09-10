#pragma once

class MovementComponent;
class TransformComponent;

class MovementSystem
{
public:
	static void HandleMovement(MovementComponent& mc, TransformComponent& tc);
};


#include "MovementSystem.h"
#include "MovementComponent.h"
#include "TransformComponent.h"
#include "Input.h"
#include <iostream>

void MovementSystem::HandleMovement(MovementComponent& mc, TransformComponent& tc)
{
	// Mixed system functionality FYFY
	mc.forward = Input::GetActionHeld("up") - Input::GetActionHeld("down");
	mc.sideways = Input::GetActionHeld("right") - Input::GetActionHeld("left");
	mc.up = Input::GetActionHeld("space") - Input::GetActionHeld("shift");
	std::cout << "Forward: " << mc.forward << ", " <<
		"Up: " << mc.up << ", " <<
		"Sideways: " << mc.sideways << std::endl;
	tc.transform = glm::translate(tc.transform, { mc.sideways, mc.up, mc.forward });
	mc.forward = 0.f;
	mc.up = 0.f;
	mc.sideways = 0.f;
}

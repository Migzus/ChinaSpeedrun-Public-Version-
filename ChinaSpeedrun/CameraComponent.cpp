#include "CameraComponent.h"

cs::CameraComponent::CameraComponent()
{
    forward = { 0.0f, 0.0f, 1.0f };
    up = { 0.0f, 0.0f, 1.0f };
    right = { 1.0f, 0.0f, 0.0f };

    angle = 45.f;
    aspect = 4.f / 3.f;
    close = 0.1f;
    distant = 1000.f;
    
    view = glm::lookAt(Vector3(2.0f, 2.0f, 2.0f), Vector3(0.0f, 0.0f, 0.0f), up);
    proj = glm::perspective(angle, aspect, close, distant);

    position = { 2.f, 2.f, 2.f };
}

#include "CameraComponent.h"

cs::CameraComponent::CameraComponent() :
	proj { glm::perspective(Mathf::PI * 0.25f, 3.f/4.f, 0.1f, 10.0f) },
	view { glm::lookAt(Vector3(2.0f), Vector3(0.0f), Vector3(0.0f, 0.0f, 1.0f)) }
{
}

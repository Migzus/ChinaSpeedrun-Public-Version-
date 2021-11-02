#include "PhysicsComponent.h"
#include "imgui.h"

#include <b2/b2_world.h>
#include <b2/b2_fixture.h>
#include <b2/b2_circle_shape.h>

#include "CollisionShape.h"
#include "PhysicsLocator.h"
#include "PhysicsSystem.h"
#include "Debug.h"

void cs::PhysicsDelta::Step(const Vector2& newPosition, float newAngle)
{
	positionDifference = newPosition - positionPrevious;
	angleDifference = newAngle - anglePrevious;
	positionPrevious = newPosition;
	anglePrevious = newAngle;
}

void cs::PhysicsDelta::Teleport(const Vector2& newPosition, float newAngle)
{
	positionDifference = { 0.f, 0.f };
	angleDifference = 0.f;
	positionPrevious = newPosition;
	anglePrevious = newAngle;
}

cs::PhysicsDelta::PhysicsDelta() : positionDifference({0.f}), angleDifference(0.f), positionPrevious({0.f}), anglePrevious(0.f)
{
}

cs::PhysicsComponent::PhysicsComponent() : body(nullptr)
{
	QueueForCreation();
	definition.awake = false;
	definition.type = b2_staticBody;
}

cs::PhysicsComponent::~PhysicsComponent()
{
	if (body)
	{
		auto _ps(PhysicsLocator::GetPhysicsSystem());
		_ps->world->DestroyBody(body);
	}
}

void cs::PhysicsComponent::ImGuiDrawComponent()
{
	/*
	 * Detecting changes in definition etc.
	 * Once scenes exist we wont need this any more
	 */
	bool _recreate(false), _update(false);

	if (ImGui::TreeNodeEx("Physics Component", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const float dragSpeed(0.1f);

		{
			const char* _options = { "Static\0Kinematic\0Dynamic" };
			ImGui::Combo("Type", (int*)&definition.type, _options);
		}

		if (body)
		{
			std::string _positionString = std::to_string(body->GetPosition().x) + ", " + std::to_string(body->GetPosition().y);
			ImGui::Text(&_positionString[0]);
		}

		const char* _options("None\0Circle\0Rectangle" );
		int _type(static_cast<int>(shape.GetType()));
		if (ImGui::Combo("Shape", &_type, _options))
		{
			CollisionShape::Type _newType(static_cast<CollisionShape::Type>(_type));
			shape.SetType(_newType);
			_recreate = true;
		}

		/*
		 * Maybe a shape should be a component (although that comes with its own implementation horrors
		 * Maybe a shape should be a resource (resources could perhaps also have their own imgui changing/drawing functions)
		 */
		switch (shape.GetType())
		{
			case CollisionShape::Type::Circle:
				ImGui::TreeNodeEx("Circle Shape", ImGuiTreeNodeFlags_DefaultOpen);
				_recreate |= ImGui::DragFloat("Radius", &shape.shape->m_radius, dragSpeed, -1000.f, 1000.f);
				ImGui::TreePop();
				break;

			case CollisionShape::Type::Rectangle:
				ImGui::TreeNodeEx("Rectangle Shape", ImGuiTreeNodeFlags_DefaultOpen);
				auto* _rectangle(reinterpret_cast<b2BoxShape*>(shape.shape));
				b2Vec2 _extents(_rectangle->GetExtents());
				if (ImGui::DragFloat2("Extents", &_extents.x, dragSpeed, -1000.f, 1000.f))
				{
					_rectangle->SetExtents(_extents);
					_recreate = true;
				}
				ImGui::TreePop();
				break;
		}

		_update |= ImGui::DragFloat("Gravity Scale", &definition.gravityScale, dragSpeed, -10.0f, 10.0f);
		_update |= ImGui::DragFloat2("Linear Velocity", &definition.linearVelocity.x, dragSpeed * 10.f, -100.f, 100.f);
		_update |= ImGui::DragFloat("Angular Velocity", &definition.angularVelocity, dragSpeed * 10.f, -100.f, 100.f);
		_update |= ImGui::Checkbox("Awake", &definition.awake);

		if (_recreate || ImGui::Button("Recreate Body"))
			QueueForCreation();
		else if (_update)
			QueueForUpdate();

		ImGui::TreePop();
	}
}

void cs::PhysicsComponent::QueueForUpdate()
{
	PhysicsLocator::GetPhysicsSystem()->QueueComponentUpdate(this);
}

void cs::PhysicsComponent::QueueForCreation()
{
	PhysicsLocator::GetPhysicsSystem()->QueueComponentCreate(this);
}

void cs::PhysicsComponent::UpdateFixture()
{
	body->DestroyFixture(&body->GetFixtureList()[0]);
	body->CreateFixture(shape.shape, 1.f);
}

#include "CollisionShape.h"

#include <b2/b2_circle_shape.h>

void cs::CollisionShape::SetType(Type newType)
{
	if (newType != type)
	{
		delete shape;

		switch (newType)
		{
		case Type::None:
		{
			shape = nullptr;
			break;
		}
		case Type::Rectangle:
		{
			auto _shape(new b2BoxShape());
			_shape->SetExtents({0.5f, 0.5f});
			shape = reinterpret_cast<b2Shape*>(_shape);
			break;
		}

		case Type::Circle:
		{
			auto _shape(new b2CircleShape());
			_shape->m_radius = 0.5f;
			shape = _shape;
			break;
		}
		}

		type = newType;
	}
}

cs::CollisionShape::CollisionShape() : shape(nullptr), type(Type::None)
{
}




b2BoxShape::b2BoxShape() : extents({0.5f, 0.5f})
{
	SetAsBox(extents.x, extents.y);
}

b2Vec2 b2BoxShape::GetExtents() const
{
	return extents;
}

void b2BoxShape::SetExtents(b2Vec2 newExtents)
{
	extents = newExtents;
	SetAsBox(extents.x, extents.y);
}

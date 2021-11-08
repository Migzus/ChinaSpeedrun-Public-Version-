#pragma once

#include "Mathf.h"
#include <b2/b2_polygon_shape.h>

// ooga booga custom class ooga booga
// this may have consequences in the future
class b2BoxShape : protected b2PolygonShape
{
public:
	b2BoxShape();
	b2Vec2 GetExtents() const;
	void SetExtents(b2Vec2 newExtents);
protected:
	b2Vec2 extents;
};

namespace cs
{
	class CollisionShape
	{
	public:
		enum class Type { None, Circle, Rectangle };

		b2Shape* shape;

		void SetType(Type newType);
		Type GetType() const { return type; }

		CollisionShape();
	protected:
		Type type;
	};
}


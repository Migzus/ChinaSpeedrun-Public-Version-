#pragma once

class TransformComponent;

namespace cs {
	class MeshRenderer;
}

class MeshRendererSystem
{
public:
	static void UpdatePosition(cs::MeshRenderer& mr, TransformComponent& tc);
};


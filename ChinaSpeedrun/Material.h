#pragma once

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <variant>

#include "Resource.h"

#include "Mathf.h"
#include "Texture.h"

namespace cs
{
	typedef std::variant<bool, int, float, Texture*, Vector2, Vector3, Vector4, Matrix3x3, Matrix4x4> Variant;

	class Material : public Resource
	{
	public:
		friend class Draw;
		friend class VulkanEngineRenderer;
		friend class ResourceManager;
		friend class MeshRendererComponent;
		friend class BulletManagerComponent;

		enum class RenderMode
		{
			OPEQUE_,
			TRANSPARENT_ // because a TRANSPARENT macro is already defined by microsoft, we have to add the underscore to avoid conflict
		} renderMode;
		
		enum class FillMode
		{
			FILL = 0,
			LINE = 1,
			POINT = 2
		} fillMode;

		enum class CullMode
		{
			NONE = 0,
			FRONT = 0x00000001,
			BACK = 0x00000002,
			FRONT_AND_BACK = 0x00000003
		} cullMode;

		class Shader* shader;
		std::unordered_map<std::string, Variant> shaderParams;
		float lineWidth;
		bool enableStencil;

		Material();

		void Initialize() override;

		~Material();

	private:
		VkPipeline pipeline;
	};
}

#pragma once

#include <vulkan/vulkan.h>

namespace cs
{
	class Texture
	{
	public:
		// we need to store pixel information
		VkDeviceSize bufferOffset;

	private:
		uint32_t width, height;
	};
}

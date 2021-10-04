#pragma once

#include "Resource.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace cs
{
	class Texture : public Resource
	{
	public:
		VkImageTiling tiling;
		uint8_t* pixels;
		uint32_t mipLevels;
		int usedColorChannels, width, height;

		Texture();

		void Initialize() override;

		unsigned char* GetRawPixels();
		std::vector<class Color> GetPixels() const;
		void SetPixels(std::vector<Color> pixels, uint32_t width, uint32_t height);
		uint32_t GetTextureByteSize() const;

	private:
		VkImage texture;
		VkDeviceMemory textureMemory;
		VkImageView textureView;
	};
}

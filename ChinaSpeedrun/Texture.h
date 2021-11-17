#pragma once

#include "Resource.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace cs
{
	class Texture : public Resource
	{
	public:
		friend class VulkanEngineRenderer;
		friend class ResourceManager;

		enum class Tiling
		{
			REPEAT = 0,
			MIRRORED_REPEAT = 1,
			CLAMP = 2
		} tilingX, tilingY;

		enum class Filter
		{
			NEAREST = 0,
			LINEAR = 1,
			CUBIC = 1000015000
		} filter;

		bool generateMipmaps;

		Texture();

		void Initialize() override;

		unsigned char* GetRawPixels();
		std::vector<class Color> GetPixels() const;
		void SetPixels(std::vector<Color> pixels, uint32_t width, uint32_t height);
		uint32_t GetTextureByteSize() const;

	private:
		uint8_t* pixels;
		uint32_t mipLevels;
		int usedColorChannels, width, height;

		VkImageView textureView;
		VkSampler textureSampler;
		VkImage texture;
		VkDeviceMemory textureMemory;
	};
}

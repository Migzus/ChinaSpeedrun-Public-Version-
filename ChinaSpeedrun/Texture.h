#pragma once

#include "Resource.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace cs
{
	class Texture : public Resource
	{
	public:
		Texture(std::string fileName, int usedChannels = 4);

		void ReadTexture(std::string fileName, int usedChannels = 4);
		unsigned char* GetRawPixels();
		std::vector<class Color> GetPixels() const;
		void SetPixels(std::vector<class Color> pixels, uint64_t width, uint64_t height);
		uint64_t GetTextureByteSize() const;

		VkDeviceSize bufferOffset, texturePixelSize;
		VkImageTiling tiling;

	private:
		char usedColorChannels;
		unsigned char* pixels;
		uint64_t width, height;
	};
}

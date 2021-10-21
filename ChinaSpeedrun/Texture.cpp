#include "Texture.h"

#include <iostream>

#include "Color.h"
#include "ChinaEngine.h"
#include "VulkanEngineRenderer.h"

cs::Texture::Texture() :
	pixels{ nullptr }
{}

void cs::Texture::Initialize()
{
	ChinaEngine::renderer.AllocateTexture(pixels, mipLevels, width, height, texture, textureMemory, textureView, textureSampler);
}

uint8_t* cs::Texture::GetRawPixels()
{
	return pixels;
}

std::vector<cs::Color> cs::Texture::GetPixels() const
{
	uint32_t _colorArrayLength{ width * height / (uint32_t)usedColorChannels };
	std::vector<Color> _pixels;

	// for now we assume 4 color channels are always used... (in the for-loop)
	// will fix this later
	for (size_t i{ 0 }; i < _colorArrayLength; i++)
	{
		size_t _index{ i * usedColorChannels };

		// We need to avoid deviding by zero... cause we just want it to be zero
		// also might add support for char sized values in color, so i can move this logic in there
		_pixels.push_back(Color(
			(pixels[_index] == 0) ? 0.0f : 255.0f / (float)pixels[_index],
			(pixels[_index + 1] == 0) ? 0.0f : 255.0f / (float)pixels[_index + 1],
			(pixels[_index + 2] == 0) ? 0.0f : 255.0f / (float)pixels[_index + 2],
			(pixels[_index + 3] == 0) ? 0.0f : 255.0f / (float)pixels[_index + 3]
		));
	}

	return _pixels;
}

void cs::Texture::SetPixels(std::vector<Color> pixels, uint32_t width, uint32_t height)
{
	// by running this we need to update the textureMemory
}

uint32_t cs::Texture::GetTextureByteSize() const
{
	return width * height;
}

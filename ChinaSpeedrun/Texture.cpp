#include "Texture.h"

#include <stb_image.h>
#include <iostream>

#include "Color.h"

using namespace cs;

Texture::Texture(std::string fileName, int usedChannels)
{
	ReadTexture(fileName, usedChannels);
}

void Texture::ReadTexture(std::string fileName, int usedChannels)
{
	int _texWidth, _texHeight, _texChannels;
	pixels = stbi_load(fileName.c_str(), &_texWidth, &_texHeight, &_texChannels, usedChannels);

	if (!pixels)
	{
		std::cout << "[WARNING]\t: Cannot load image, image \"" << fileName << "\" does not exist.";
		return;
	}

	usedColorChannels = usedChannels;
	width = static_cast<uint64_t>(_texWidth);
	height = static_cast<uint64_t>(_texHeight);

	texturePixelSize = static_cast<uint64_t>(_texWidth * _texHeight * usedChannels);
}

stbi_uc* cs::Texture::GetRawPixels()
{
	return pixels;
}

std::vector<class Color> cs::Texture::GetPixels() const
{
	uint64_t _colorArrayLength{ width * height / usedColorChannels };
	std::vector<class Color> _pixels;

	// for now we assume 4 color channels are always used... (in the for-loop)
	// will fix this later
	for (size_t i{ 0 }; i < _colorArrayLength; i++)
	{
		// We need to avoid deviding by zero... cause we just want it to be zero
		// also might add support for char sized values in color, so i can move this logic in there
		_pixels.push_back(Color(
			(pixels[i] == 0) ? 0.0f : 255.0f / (float)pixels[i],
			(pixels[i + 1] == 0) ? 0.0f : 255.0f / (float)pixels[i + 1],
			(pixels[i + 2] == 0) ? 0.0f : 255.0f / (float)pixels[i + 2],
			(pixels[i + 3] == 0) ? 0.0f : 255.0f / (float)pixels[i + 3]
		));
	}

	return _pixels;
}

void cs::Texture::SetPixels(std::vector<class Color> pixels, uint64_t width, uint64_t height)
{

}

uint64_t cs::Texture::GetTextureByteSize() const
{
	return width * height;
}

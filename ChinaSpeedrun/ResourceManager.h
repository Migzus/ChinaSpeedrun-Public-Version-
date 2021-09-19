#pragma once

#include <string>

namespace cs
{
	class ResourceManager
	{
	public:
		template<typename T>
		static T* Load(const std::string filename);
		static class Mesh* LoadModel(const std::string filename);
		static class AudioSource* LoadAudio(const std::string filename);
		static class Texture* LoadImage(const std::string filename);
		static class Shader* LoadShader(const std::string filename);
		static class Material* LoadMaterial(const std::string filename);
		static class Scene* LoadScene(const std::string filename);
		static std::vector<uint8_t> LoadRaw(const std::string filename);
	};
}

#pragma once

#include <string>
#include <map>
#include <iostream>
#include <vector>

namespace cs
{
	class ResourceManager
	{
	public:
		template<class T>
		static T* Load(const std::string filename);
		static class Mesh* LoadModel(const std::string filename);
		static class AudioSource* LoadAudio(const std::string filename);
		static class Texture* LoadTexture(const std::string filename);
		static class Shader* LoadShader(const std::string filename);
		static class Material* LoadMaterial(const std::string filename);
		static class Scene* LoadScene(const std::string filename); // scene would also be a prefab
		static std::vector<char> LoadRaw(const std::string filename);

		// It is EXTREMELY dangerous to call this function from a normal script in-game. NEVER do this.
		static void ClearAllResourcePools();

	private:
		static std::map<std::string, cs::Mesh*> meshes;
		static std::map<std::string, cs::AudioSource*> audioTracks;
		static std::map<std::string, cs::Texture*> textures;
		static std::map<std::string, cs::Shader*> shaders;
		static std::map<std::string, cs::Material*> materials;
	};

	template<class T>
	inline T* ResourceManager::Load(const std::string filename)
	{
		/*if (reinterpret_cast<Texture>(T) == Texture)
			std::cout << "Texture" << '\n';
		else if (reinterpret_cast<Mesh>(T) == Mesh)
			std::cout << "Mesh" << '\n';*/

		// Otherwise the class spesifier was invalid, thus returning null
		return nullptr;
	}
}

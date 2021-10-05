#pragma once

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <fstream>

typedef std::vector<char> RawData;

namespace cs
{
	class ResourceManager
	{
	public:
		template<class T>
		static T* IsDuplicateResource(std::string filename);
		template<class T>
		static T* Load(const std::string filename);
		template<class T>
		static T* Save(const std::string filename, T* resource);
		template<class T>
		static void ForcePush(T* resource);

		static class Mesh* LoadModel(const std::string filename);
		static class AudioSource* LoadAudio(const std::string filename);
		static class Texture* LoadTexture(const std::string filename);
		static class Shader* LoadShader(std::vector<std::string> filenames);
		static class Material* LoadMaterial(const std::string filename);
		static class Scene* LoadScene(const std::string filename); // scene would also be a prefab
		static RawData LoadRaw(const std::string filename);

		static void ForcePushMesh(Mesh* mesh);

		/////////////
		static void InitializeTest();
		/////////////

		template<>
		static Mesh* IsDuplicateResource(std::string filename)
		{
			return meshes[filename];
		}
		template<>
		static Mesh* Load(const std::string filename)
		{
			return LoadModel(filename);
		}

		template<>
		static void ForcePush(Mesh* mesh)
		{
			ForcePushMesh(mesh);
		}

		template<>
		static Texture* IsDuplicateResource(std::string filename)
		{
			return textures[filename];
		}
		template<>
		static Texture* Load(const std::string filename)
		{
			return LoadTexture(filename);
		}

		template<>
		static Material* IsDuplicateResource(std::string filename)
		{
			return materials[filename];
		}
		template<>
		static Material* Load(const std::string filename)
		{
			return LoadMaterial(filename);
		}

		template<>
		static Shader* IsDuplicateResource(std::string filename)
		{
			return shaders[filename];
		}
		template<>
		static Shader* Load(const std::string filename)
		{
			const std::string _shaderTypes[]{ "vert", "frag", "comp" };
			std::vector<std::string> _shaderfiles;

			for (size_t i{ 0 }; i < 3; i++)
			{
				std::string _fullfilePath{ filename + '.' + _shaderTypes[i] };
				std::ifstream _file{ _fullfilePath };

				if (_file.good())
					_shaderfiles.push_back(_fullfilePath);
			}

			return LoadShader(_shaderfiles);
		}

		// It is EXTREMELY dangerous to call this function from a normal script in-game. NEVER do this.
		static void ClearAllResourcePools();

	private:
		static std::map<std::string, cs::Mesh*> meshes;
		static std::map<std::string, cs::AudioSource*> audioTracks;
		static std::map<std::string, cs::Texture*> textures;
		static std::map<std::string, cs::Shader*> shaders;
		static std::map<std::string, cs::Material*> materials;
	};
}

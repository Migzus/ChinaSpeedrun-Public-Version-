#include "ResourceManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"

#include <fstream>

std::map<std::string, cs::Material*> cs::ResourceManager::materials;
std::map<std::string, cs::Texture*> cs::ResourceManager::textures;
std::map<std::string, cs::Mesh*> cs::ResourceManager::meshes;
std::map<std::string, cs::Shader*> cs::ResourceManager::shaders;
std::map<std::string, cs::AudioSource*> cs::ResourceManager::audioTracks;

cs::Mesh* cs::ResourceManager::LoadModel(const std::string filename)
{
	return nullptr;
}

cs::Texture* cs::ResourceManager::LoadTexture(const std::string filename)
{
	return nullptr;
}

cs::Shader* cs::ResourceManager::LoadShader(const std::string filename)
{
	return nullptr;
}

cs::Material* cs::ResourceManager::LoadMaterial(const std::string filename)
{
	return nullptr;
}

std::vector<char> cs::ResourceManager::LoadRaw(const std::string filename)
{
	std::ifstream _file{ filename, std::ios::ate | std::ios::binary };

	if (!_file.is_open())
		throw std::runtime_error("[FAIL]\t: Failed to open " + filename);

	size_t _fileSize{ (size_t)_file.tellg() };
	std::vector<char> _buffer(_fileSize);

	_file.seekg(0);
	_file.read(_buffer.data(), _fileSize);

	_file.close();

	return _buffer;
}

void cs::ResourceManager::ClearAllResourcePools()
{
	for (const std::pair<std::string, Texture*> texture : textures)
		delete texture.second;
	textures.clear();

	for (const std::pair<std::string, Mesh*> mesh : meshes)
		delete mesh.second;
	meshes.clear();

	for (const std::pair<std::string, Shader*> shader : shaders)
		delete shader.second;
	shaders.clear();

	for (const std::pair<std::string, Material*> material : materials)
		delete material.second;
	materials.clear();
}

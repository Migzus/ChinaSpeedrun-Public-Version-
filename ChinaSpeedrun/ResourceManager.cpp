#include "ResourceManager.h"

#include "Debug.h"

#include "ChinaEngine.h"
#include "VulkanEngineRenderer.h"

#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "AudioData.h"
#include "Script.h"
#include "Mathf.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fstream>
#include <AudioFile.h>

#ifdef NDEBUG
#include <shaderc/shaderc.hpp>
#endif

std::unordered_map<std::string, cs::Material*> cs::ResourceManager::materials;
std::unordered_map<std::string, cs::Texture*> cs::ResourceManager::textures;
std::unordered_map<std::string, cs::Mesh*> cs::ResourceManager::meshes;
std::unordered_map<std::string, cs::Shader*> cs::ResourceManager::shaders;
std::unordered_map<std::string, cs::AudioData*> cs::ResourceManager::audio;
std::unordered_map<std::string, cs::Script*> cs::ResourceManager::scripts;

std::vector<Vector3> cs::ResourceManager::LoadLAS(const std::string& filename, const Vector3& offset, const AxisMode& mode)
{
	std::ifstream _file{ filename };
	std::vector<Vector3> _reservedPoints;
	std::string _currentLine{};

	if (_file.bad())
		return _reservedPoints;

	std::getline(_file, _currentLine);

	const int _size{ std::stoi(_currentLine) };

	for (int i{ 0 }; i < _size; i++)
	{
		std::getline(_file, _currentLine);

		uint32_t _startStringIndex{ 0 };
		float _x{ -offset.x }, _y{ -offset.y }, _z{ -offset.z };

		_x += std::stof(_currentLine.substr(_startStringIndex, _currentLine.find_first_of('\t')));
		_startStringIndex = _currentLine.find_first_of('\t');

		std::string _midSubStr{ _currentLine.substr(_startStringIndex) };
		_y += std::stof(_currentLine.substr(_startStringIndex, _currentLine.find_first_of('\t')));
		_startStringIndex += _midSubStr.find_first_of('\t', _startStringIndex);

		_z += std::stof(_currentLine.substr(_startStringIndex));

		switch (mode)
		{
		case AxisMode::X:
			_reservedPoints.push_back(Vector3(_z, _x, _y));
			break;
		case AxisMode::Y:
			_reservedPoints.push_back(Vector3(_x, _z, _y));
			break;
		case AxisMode::Z:
			_reservedPoints.push_back(Vector3(_x, _y, _z));
			break;
		}
	}

	return _reservedPoints;
}

cs::Mesh* cs::ResourceManager::LoadModel(const std::string filename)
{
	Mesh* _outMesh{ IsDuplicateResource<Mesh>(filename) };

	if (_outMesh == nullptr)
	{
		_outMesh = new Mesh;

		tinyobj::attrib_t _attributes;
		std::vector<tinyobj::shape_t> _shapes;
		std::vector<tinyobj::material_t> _materials; // currently we don't do anything with the materials, but in the future we will automatically make the materials (maybe)
		std::string _warning, _error;

		if (!tinyobj::LoadObj(&_attributes, &_shapes, &_materials, &_warning, &_error, filename.c_str()))
		{
			Debug::LogWarning(_warning + _error);
			delete _outMesh;
			return nullptr;
		}

		std::vector<Vertex> _vertices;
		std::vector<uint32_t> _indices;
		// by using an unordered_map we can remove duplicates
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : _shapes)
		{
			// in the future when we have multiple shapes, make them into separate objects
			for (const auto& index : shape.mesh.indices)
			{
				Vertex _vertex{};

				_vertex.position =
				{
					_attributes.vertices[3 * index.vertex_index + 0],
					_attributes.vertices[3 * index.vertex_index + 1],
					_attributes.vertices[3 * index.vertex_index + 2]
				};

				_vertex.texCoord =
				{
					_attributes.texcoords[2 * index.texcoord_index + 0],
					1.0f - _attributes.texcoords[2 * index.texcoord_index + 1]
				};

				_vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(_vertex) == 0)
				{
					uniqueVertices[_vertex] = static_cast<uint32_t>(_vertices.size());
					_vertices.push_back(_vertex);
				}

				_indices.push_back(uniqueVertices[_vertex]);
			}
		}

		_outMesh->SetMesh(_vertices, _indices);
		_outMesh->resourcePath = filename;
		meshes[filename] = _outMesh;

		//_outMesh->Initialize();
	}

	return _outMesh;
}

cs::AudioData* cs::ResourceManager::LoadAudio(const std::string filename)
{
	AudioFile<float> _file;

	std::vector<uint8_t> _buffer;

	if (_file.loadBuffer(filename, _buffer))
	{
		const auto _audioData{ new AudioData(
			_buffer,
			AudioMeta(static_cast<float>(_buffer.size()) /
				static_cast<float>(_file.getBitDepth()) /
				static_cast<float>(_file.getNumChannelsAsRead()) /
				static_cast<float>(_file.getSampleRate()) * 8.f,
			_file.getSampleRate(),
			_file.getBitDepth(),
			_file.getNumChannelsAsRead()))};
		audio.insert({ filename, _audioData });
		return _audioData;
	}

	return nullptr;
}

cs::Texture* cs::ResourceManager::LoadTexture(const std::string filename)
{
	Texture* _outTexture{ IsDuplicateResource<Texture>(filename) };

	if (_outTexture == nullptr)
	{
		_outTexture = new Texture;
		_outTexture->pixels = stbi_load(filename.c_str(), &_outTexture->width, &_outTexture->height, &_outTexture->usedColorChannels, STBI_rgb_alpha);

		if (_outTexture->pixels == nullptr)
		{
			Debug::LogWarning("Cannot open file : [" + filename + ']');
			return nullptr;
		}
		
		_outTexture->mipLevels = static_cast<uint32_t>(std::floor(std::log2(_outTexture->width > _outTexture->height ? _outTexture->width : _outTexture->height))) + 1;
		_outTexture->resourcePath = filename;
		textures[filename] = _outTexture;

		//_outTexture->Initialize();
	}

	return _outTexture;
}

cs::Shader* cs::ResourceManager::LoadShader(std::vector<std::string> filenames)
{
	std::unordered_map<std::string, RawData> _outShaderSPVs;

	for (std::string file : filenames)
	{
		std::string _shaderType{ file.substr(file.find_last_of('.') + 1) };

#ifdef NDEBUG
		shaderc_shader_kind _shaderKind{ shaderc_glsl_default_anyhit_shader };

		if (_shaderType == "vert")
			_shaderKind = shaderc_glsl_default_vertex_shader;
		else if (_shaderType == "frag")
			_shaderKind = shaderc_glsl_default_fragment_shader;
		else if (_shaderType == "comp")
			_shaderKind = shaderc_glsl_default_compute_shader;
		else if (_shaderType == "geom")
			_shaderKind = shaderc_glsl_default_geometry_shader;
		else
		{
			Debug::LogWarning("The shader type \"" + _shaderType + "\" is either not supported or does not exist.");
			break;
		}

		shaderc::Compiler _compiler;
		std::string _outText;

		shaderc::SpvCompilationResult _result{ _compiler.CompileGlslToSpv(_outText, shaderc_glsl_default_vertex_shader, file.c_str()) };

		if (_result.GetCompilationStatus() != shaderc_compilation_status_success)
			Debug::LogWarning(_result.GetErrorMessage());
		else
			_outShaderSPVs[_shaderType] = { _outText.begin(), _outText.end() };
#else
		std::string _newFilepath{ file };

		if (_shaderType != "vert" && _shaderType != "frag" && _shaderType != "geom" && _shaderType != "comp")
		{
			Debug::LogWarning("The shader type \"" + _shaderType + "\" is either not supported or does not exist.");
			break;
		}

		_newFilepath.insert(_newFilepath.find_last_of('.'), '_' + _shaderType);
		_newFilepath.replace(_newFilepath.find_last_of('.') + 1, _newFilepath.length(), "spv");

		_outShaderSPVs[_shaderType] = LoadRaw(_newFilepath);		
#endif // NDEBUG
	}

	std::string _shaderResource{ filenames[0].substr(0, filenames[0].find_last_of('.')) };

	shaders[_shaderResource] = new Shader(_outShaderSPVs);
	shaders[_shaderResource]->resourcePath = _shaderResource;

	return shaders[_shaderResource];
}

cs::Material* cs::ResourceManager::LoadMaterial(const std::string filename)
{
	// We don't have our own material formatter or reader... sooooo, empty...
	materials[filename] = new Material;

	materials[filename]->resourcePath = filename;

	return materials[filename];
}

cs::Scene* cs::ResourceManager::LoadScene(const std::string filename)
{
	return nullptr;
}

RawData cs::ResourceManager::LoadRaw(const std::string filename)
{
	std::ifstream _file{ filename, std::ios::ate | std::ios::binary };

	if (!_file.is_open())
	{
		Debug::LogWarning("Could not open: " + filename);
		return std::vector<char>();
	}

	size_t _fileSize{ (size_t)_file.tellg() };
	std::vector<char> _buffer(_fileSize);

	_file.seekg(0);
	_file.read(_buffer.data(), _fileSize);

	_file.close();

	return _buffer;
}

cs::Script* cs::ResourceManager::LoadScript(const std::string filename)
{
	std::ifstream _readFile{ filename };

	if (_readFile.bad())
	{
		_readFile.close();
		return nullptr;
	}

	Script* _newScript{ new Script };
	_newScript->scriptText = std::string(std::istreambuf_iterator<char>(_readFile), std::istreambuf_iterator<char>());
	_readFile.close();
	_newScript->resourcePath = filename;
	return _newScript;
}

void cs::ResourceManager::SaveScript(const std::string& filename, Script* script)
{
	std::ofstream _file{ filename };

	_file << script->scriptText;
	_file.close();
}

void cs::ResourceManager::ForcePushMesh(Mesh* mesh)
{
	meshes[mesh->resourcePath] = mesh;
}

void cs::ResourceManager::ClearAllResourcePools()
{
	for (const std::pair<std::string, Texture*> texture : textures)
		ChinaEngine::renderer.SolveTexture(texture.second, Solve::REMOVE);
	textures.clear();

	for (const std::pair<std::string, Mesh*> mesh : meshes)
		ChinaEngine::renderer.SolveMesh(mesh.second, Solve::REMOVE);
	meshes.clear();

	for (const std::pair<std::string, Shader*> shader : shaders)
		ChinaEngine::renderer.SolveShader(shader.second, Solve::REMOVE);
	shaders.clear();

	for (const std::pair<std::string, Material*> material : materials)
		ChinaEngine::renderer.SolveMaterial(material.second, Solve::REMOVE);
	materials.clear();

	/*for (const std::pair<std::string, AudioData*> audio : audioTracks)
		delete audio.second;
	audioTracks.clear();*/

	for (const std::pair<std::string, Script*> script : scripts)
		delete script.second;
	scripts.clear();
}

#include "ChinaEngine.h"

//#include <chrono>

#include "Vertex.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshRenderer.h"
#include "VulkanEngineRenderer.h"

using namespace cs;

VulkanEngineRenderer ChinaEngine::renderer;
std::vector<Shader*> ChinaEngine::shaders;
std::vector<Material*> ChinaEngine::materials;
std::vector<Texture*> ChinaEngine::textures;
std::vector<Mesh*> ChinaEngine::meshes;
std::vector<MeshRenderer*> ChinaEngine::objects;

VulkanEngineRenderer const& ChinaEngine::GetVulkanRenderer()
{
	return renderer;
}

std::vector<char> ChinaEngine::ReadFile(const std::string& filename)
{
	std::ifstream _file{ filename, std::ios::ate | std::ios::binary };

	if (!_file.is_open())
		throw std::runtime_error("[FAIL] :\tFailed to open " + filename);

	size_t _fileSize{ (size_t)_file.tellg() };
	std::vector<char> _buffer(_fileSize);

	_file.seekg(0);
	_file.read(_buffer.data(), _fileSize);

	_file.close();

	return _buffer;
}

void ChinaEngine::Run()
{
	EngineInit();
	renderer.Create(800, 600, "China Speedrun");
	MainLoop();
}

std::vector<MeshRenderer*> const& ChinaEngine::GetObjects()
{
	return objects;
}

std::vector<Mesh*> const& ChinaEngine::GetMeshes()
{
	return meshes;
}

void ChinaEngine::EngineInit()
{
	Shader* _shader{ new Shader({ "../Resources/shaders/vert.spv", "../Resources/shaders/frag.spv" }) };
	Material* _material{ new Material(_shader) };

	Mesh* _mesh1{ Mesh::CreateDefaultCube({0.1f, 0.1f, 1.0f}) };
	Mesh* _mesh2{ Mesh::CreateDefaultPlane({0.5f, 0.5f}) };

	_mesh1->materials.push_back(_material);
	_mesh2->materials.push_back(_material);

	meshes.push_back(_mesh1);
	meshes.push_back(_mesh2);

	MeshRenderer* _renderer1{ new MeshRenderer(_mesh1) };
	MeshRenderer* _renderer2{ new MeshRenderer(_mesh2) };

	_renderer1->ubo = new UniformBufferObject;
	_renderer2->ubo = new UniformBufferObject;

	_renderer1->ubo->model = glm::translate(Matrix4x4(1.0f), Vector3(0.0f, 0.0f, 0.0f));
	_renderer1->ubo->view = glm::lookAt(Vector3(2.0f, 2.0f, 2.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
	_renderer1->ubo->proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10.0f);
	_renderer1->ubo->proj[1][1] *= -1;

	_renderer2->ubo->model = glm::translate(Matrix4x4(1.0f), Vector3(0.0f, 0.0f, 0.0f));
	_renderer2->ubo->view = _renderer1->ubo->view;
	_renderer2->ubo->proj = _renderer1->ubo->proj;

	// currently we assign this ourselves, but we will move it into an automatic offset assginer
	_renderer1->uboOffset = 0;
	_renderer2->uboOffset = sizeof(UniformBufferObject);

	objects.push_back(_renderer1);
	objects.push_back(_renderer2);
}

void ChinaEngine::MainLoop()
{
	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		glfwPollEvents();
		renderer.DrawFrame();
	}

	vkDeviceWaitIdle(renderer.GetDevice());
}

void ChinaEngine::EngineExit()
{
	for (auto object : objects)
		delete object;

	for (auto mesh : meshes)
		delete mesh;

	for (auto texture : textures)
		delete texture;

	for (auto material : materials)
		delete material;

	for (auto shader : shaders)
		delete shader;
}

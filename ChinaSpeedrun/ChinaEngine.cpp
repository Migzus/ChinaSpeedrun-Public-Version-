#include "ChinaEngine.h"

//#include <chrono>

#include "Vertex.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshRenderer.h"
#include "VulkanEngineRenderer.h"

#include "Input.h"
#include "World.h"
#include "MovementComponent.h"
#include "CameraComponent.h"

using namespace cs;

World* ChinaEngine::world;
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

	for (auto mesh : meshes)
		renderer.AllocateMesh(mesh);

	// Temporary solution to a visual glitch
	renderer.Redraw();

	InitInput();

	MainLoop();
	EngineExit();
}

std::vector<MeshRenderer*> const& ChinaEngine::GetObjects()
{
	return objects;
}

std::vector<Mesh*> const& ChinaEngine::GetMeshes()
{
	return meshes;
}

void ChinaEngine::InstanceObject(Mesh* mesh, Material* material)
{

}

float cs::ChinaEngine::AspectRatio()
{
	return renderer.AspectRatio();
}

void ChinaEngine::EngineInit()
{
	world = new World;

	Shader* _shader{ new Shader({ "../Resources/shaders/vert.spv", "../Resources/shaders/frag.spv" }) };
	Material* _material{ new Material(_shader) };

	Texture* _vargFlush{ new Texture("../Resources/textures/varg_flush.png") };
	Texture* _junkoGyate{ new Texture("../Resources/textures/junko_gyate.png") };

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

void cs::ChinaEngine::InitInput()
{
	glfwSetKeyCallback(renderer.GetWindow(), Input::GlfwKeyfunCallback);
	Input::AddMapping("accept", GLFW_KEY_ENTER);
	Input::AddMapping("space", GLFW_KEY_SPACE);
	Input::AddMapping("shift", GLFW_KEY_LEFT_SHIFT);
	Input::AddMapping("up", GLFW_KEY_UP);
	Input::AddMapping("down", GLFW_KEY_DOWN);
	Input::AddMapping("left", GLFW_KEY_LEFT);
	Input::AddMapping("right", GLFW_KEY_RIGHT);
}

void ChinaEngine::MainLoop()
{
	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		glfwPollEvents();

		world->Step();

		CameraComponent& cc = world->GetCameraComponent();
		for (auto _obj : objects) {
			_obj->ubo->proj = cc.proj *= -1;
			_obj->ubo->view = cc.view;
		}

		for (size_t i{ 0 }; i < objects.size(); i++)
			objects[i]->Update(i);

		renderer.DrawFrame();
		Input::FinishFrame();
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

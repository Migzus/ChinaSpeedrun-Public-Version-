#include "ChinaEngine.h"

#include "AudioComponent.h"
#include "ResourceManager.h"

#include "Vertex.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshRenderer.h"
#include "VulkanEngineRenderer.h"

#include "ImGuiLayer.h"

#include "Input.h"
#include "World.h"
#include "Transform.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "Camera.h"
#include "PhysicsComponent.h"

#include "Time.h"

cs::ImGuiLayer cs::ChinaEngine::imGuiLayer;
cs::World cs::ChinaEngine::world;
cs::VulkanEngineRenderer cs::ChinaEngine::renderer;

void cs::ChinaEngine::Run()
{
	Time::CycleInit();

	renderer.Create(800, 600, "China Speedrun");

	EngineInit();
	renderer.Resolve();

	imGuiLayer.SetStyle();
	InitInput();

	MainLoop();
	EngineExit();
}

float cs::ChinaEngine::AspectRatio()
{
	return renderer.AspectRatio();
}

void cs::ChinaEngine::EngineInit()
{
	// For future notice... move this entire function somewhere else...
	// the head ChinaEngine class has nothing to do with instancing objects (maybe in a scene loader or something...)

	Shader* _shader{ ResourceManager::Load<Shader>("../Resources/shaders/default_shader") };
	_shader->AssignShaderVertexInputAttrib("position", 0, Shader::Data::VEC3, offsetof(Vertex, position));
	_shader->AssignShaderVertexInputAttrib("color", 1, Shader::Data::VEC3, offsetof(Vertex, color));
	_shader->AssignShaderVertexInputAttrib("texCoord", 2, Shader::Data::VEC2, offsetof(Vertex, texCoord));

	_shader->AssignShaderDescriptor("ubo", 0, Shader::Type::VERTEX, Shader::Data::UNIFORM);
	_shader->AssignShaderDescriptor("texSampler", 1, Shader::Type::FRAGMENT, Shader::Data::SAMPLER2D);

	Texture* _junkoGyate{ ResourceManager::Load<Texture>("../Resources/textures/junko_gyate.png") };
	Texture* _chaikaSmile{ ResourceManager::Load<Texture>("../Resources/textures/chaika_smile.png") };

	Material* _material1{ ResourceManager::Load<Material>("../Resources/materials/test1.mat") };
	Material* _material2{ ResourceManager::Load<Material>("../Resources/materials/test2.mat") };

	_material1->shader = _shader;
	_material1->shaderParams["texSampler"] = _junkoGyate;

	_material2->shader = _shader;
	_material2->cullMode = Material::CullMode::NONE;
	_material2->shaderParams["texSampler"] = _chaikaSmile;

	GameObject* _suzanne{ world.InstanceObject("Suzanne", Vector3(0.0f, 10.0f, 4.0f)) };
	GameObject* _underSuzanne{ world.InstanceObject("Under Suzanne", {0.f, 0.f, 5.f}) };
	GameObject* _camera{ world.InstanceObject("Camera", Vector3(13.0f, 13.0f, 16.0f), Vector3(-33.0f, 35.0f, 0.0f)) };

	_suzanne->AddComponent<PhysicsComponent>();
	_underSuzanne->AddComponent<PhysicsComponent>();

	MeshRendererComponent& _meshRendererMonke{ _suzanne->AddComponent<MeshRendererComponent>() };
	_meshRendererMonke.mesh = ResourceManager::Load<Mesh>("../Resources/models/suzanne.obj");
	_meshRendererMonke.materials.push_back(_material1);

	MeshRendererComponent& _meshRendererIcosphere{ _underSuzanne->AddComponent<MeshRendererComponent>() };
	_meshRendererIcosphere.mesh = ResourceManager::Load<Mesh>("../Resources/models/icosphere.obj");
	_meshRendererIcosphere.materials.push_back(_material1);

	CameraComponent& _cameraComponent{ _camera->AddComponent<CameraComponent>() };
	CameraComponent::currentActiveCamera = &_cameraComponent;
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

void cs::ChinaEngine::MainLoop()
{
	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		Time::CycleStart();

		glfwPollEvents();

		imGuiLayer.Begin();
		imGuiLayer.Step();
		imGuiLayer.End();

		world.Step();

		renderer.Resolve();
		renderer.DrawFrame();
		Input::FinishFrame();

		Time::CycleEnd();
	}

	vkDeviceWaitIdle(renderer.GetDevice());
}

void cs::ChinaEngine::EngineExit()
{
	world.DeleteAllObjects();
	ResourceManager::ClearAllResourcePools();
}

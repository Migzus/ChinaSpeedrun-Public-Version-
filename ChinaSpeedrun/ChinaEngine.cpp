#include "ChinaEngine.h"

#include "ResourceManager.h"

#include "Vertex.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshRenderer.h"
#include "VulkanEngineRenderer.h"

#include "Input.h"
#include "World.h"
//#include "MovementComponent.h"
//#include "CameraComponent.h"
#include "Transform.h"
//#include "AudioSystem.h"
#include "GameObject.h"

#include "Time.h"

cs::World cs::ChinaEngine::world;
cs::VulkanEngineRenderer cs::ChinaEngine::renderer;
std::vector<cs::GameObject*> cs::ChinaEngine::objects;

void cs::ChinaEngine::Run()
{
	Time::CycleInit();

	EngineInit();

	renderer.Create(800, 600, "China Speedrun");

	// Temporarly pre init all resources (so vulkan can actually use it)
	ResourceManager::InitializeTest();

	// Temporary solution to a visual glitch
	renderer.Redraw();

	InitInput();
	ImGuiStyleInit();

	MainLoop();
	EngineExit();
}

std::vector<cs::GameObject*> const& cs::ChinaEngine::GetObjects()
{
	return objects;
}

cs::GameObject* cs::ChinaEngine::InstanceObject(const char* name, const Vector3 position, const Vector3 rotation, const Vector3 scale)
{
	GameObject* _newObject{ new GameObject };
	_newObject->entity = world.registry.create();
	_newObject->name = name;

	TransformComponent& _transform{ _newObject->AddComponent<TransformComponent>() };
	_transform.position = position;
	_transform.rotation = rotation;
	_transform.scale = scale;

	objects.push_back(_newObject);
	return _newObject;
}

float cs::ChinaEngine::AspectRatio()
{
	return renderer.AspectRatio();
}

void cs::ChinaEngine::EngineInit()
{
	Shader* _shader{ ResourceManager::Load<Shader>("../Resources/shaders/default_shader") };
	Material* _material{ ResourceManager::Load<Material>("../Resources/materials/test.mat") };

	//_material->shader = _shader;

	// these make no difference when spawning an object... yet
	// meaning you can't assign these textures to any models...
	Texture* _vargFlush{ ResourceManager::Load<Texture>("../Resources/textures/varg_flush.png") };
	Texture* _junkoGyate{ ResourceManager::Load<Texture>("../Resources/textures/junko_gyate.png") };
	Texture* _chaikaSmile{ ResourceManager::Load<Texture>("../Resources/textures/chaika_smile.png") };

	// for now we're just creating objects like this... will change this in the future
	GameObject* _cube{ InstanceObject("Cube", Vector3(-1.3f, 0.0f, 1.2f)) };
	GameObject* _plane{ InstanceObject("Plane", Vector3(-0.45f, 0.7f, 0.0f)) };
	GameObject* _suzanne{ InstanceObject("Suzanne") };

	MeshRendererComponent& _meshRendererCube{ _cube->AddComponent<MeshRendererComponent>() };
	_meshRendererCube.mesh = Mesh::CreateDefaultCube({ 0.1f, 0.1f, 1.0f });
	_meshRendererCube.materials.push_back(_material);

	MeshRendererComponent& _meshRendererPlane{ _plane->AddComponent<MeshRendererComponent>() };
	_meshRendererPlane.mesh = Mesh::CreateDefaultPlane({ 0.5f, 0.5f });
	_meshRendererPlane.materials.push_back(_material);

	MeshRendererComponent& _meshRendererMonke{ _suzanne->AddComponent<MeshRendererComponent>() };
	_meshRendererMonke.mesh = ResourceManager::Load<Mesh>("../Resources/models/suzanne.obj");
	_meshRendererMonke.materials.push_back(_material);
}

void cs::ChinaEngine::ImGuiStyleInit()
{
	/// Here we can change the style of ImGui however we like.
	ImGuiStyle* _style{ &ImGui::GetStyle() };
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

void cs::ChinaEngine::ImGuiDraw()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (ImGui::Begin("Hierarchy"))
	{
		if (ImGui::TreeNode("Main Scene"))
		{
			for (GameObject* object : objects)
			{
				ImGui::Text(object->name.c_str());
				if (ImGui::IsItemHovered())
					ImGui::Text("Hello, I am hovering over %s", object->name.c_str());
			}
			ImGui::TreePop();
		}
	}
	
	ImGui::End();

	if (ImGui::Begin("Inspector"))
	{
		
	}
	ImGui::End();

	ImGui::Render();
}

void cs::ChinaEngine::MainLoop()
{
	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		Time::CycleStart();

		glfwPollEvents();

		ImGuiDraw();

		world.Step();

		//for (size_t i{ 0 }; i < objects.size(); i++)
		//	objects[i]->Update();

		renderer.DrawFrame();
		Input::FinishFrame();

		Time::CycleEnd();
	}

	vkDeviceWaitIdle(renderer.GetDevice());
}

void cs::ChinaEngine::EngineExit()
{
	for (auto object : objects)
		delete object;

	ResourceManager::ClearAllResourcePools();
}

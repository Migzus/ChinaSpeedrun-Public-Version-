#include "ChinaEngine.h"

#include "ResourceManager.h"

#include "Vertex.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshRenderer.h"
#include "VulkanEngineRenderer.h"

#include "ImGuizmo.h"

#include "Input.h"
#include "World.h"
#include "Transform.h"
#include "GameObject.h"
#include "CameraComponent.h"

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

	ImGuiStyleInit();
	InitInput();

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
	_transform.rotationDegrees = rotation;
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
	GameObject* _suzanne{ InstanceObject("Suzanne", Vector3(0.0f, -2.0f, -1.0f), Vector3(90.0f, 0.0f, 0.0f)) };
	GameObject* _randomObject{ InstanceObject("This object has no renderer") };
	GameObject* _camera{ InstanceObject("Camera", Vector3(0.0f, 1.0f, -6.0f), Vector3(62.0f, -23.0f, 16.0f)) };

	MeshRendererComponent& _meshRendererCube{ _cube->AddComponent<MeshRendererComponent>() };
	_meshRendererCube.mesh = Mesh::CreateDefaultCube({ 0.1f, 0.1f, 1.0f });
	_meshRendererCube.materials.push_back(_material);

	MeshRendererComponent& _meshRendererPlane{ _plane->AddComponent<MeshRendererComponent>() };
	_meshRendererPlane.mesh = Mesh::CreateDefaultPlane({ 0.5f, 0.5f });
	_meshRendererPlane.materials.push_back(_material);

	MeshRendererComponent& _meshRendererMonke{ _suzanne->AddComponent<MeshRendererComponent>() };
	_meshRendererMonke.mesh = ResourceManager::Load<Mesh>("../Resources/models/suzanne.obj");
	_meshRendererMonke.materials.push_back(_material);

	CameraComponent& _cameraComponent{ _camera->AddComponent<CameraComponent>() };
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
	ImGuizmo::BeginFrame();

	static GameObject* _activeObject{ nullptr };

	if (ImGui::Begin("Hierarchy"))
	{
		if (ImGui::TreeNode("Main Scene"))
		{
			for (GameObject* object : objects)
			{
				ImGui::Text(object->name.c_str());
				if (ImGui::IsItemClicked())
					_activeObject = object;
			}
			ImGui::TreePop();
		}
	}
	
	ImGui::End();

	if (ImGui::Begin("Inspector"))
	{
		if (_activeObject != nullptr)
		{
			ImGui::Text(_activeObject->name.c_str());

			// in the future we will move this entire draw function for ImGui to another class
			// and drawing components in the inspector will not be done by multiple if statements...
			if (_activeObject->HasComponent<TransformComponent>())
			{
				if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					TransformComponent& _transform{ world.registry.get<TransformComponent>(_activeObject->entity) };

					ImGui::DragFloat3("Position", &_transform.position.x);
					ImGui::DragFloat3("Rotation", &_transform.rotationDegrees.x);
					ImGui::DragFloat3("Scale", &_transform.scale.x);

					ImGui::TreePop();
				}
			}

			if (_activeObject->HasComponent<MeshRendererComponent>())
			{
				if (ImGui::TreeNodeEx("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
				{
					

					ImGui::TreePop();
				}
			}

			if (_activeObject->HasComponent<CameraComponent>())
			{
				if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen))
				{
					CameraComponent& _camera{ world.registry.get<CameraComponent>(_activeObject->entity) };

					ImGui::DragFloat("Field of View", &_camera.fov);
					ImGui::DragFloat("Near Plane", &_camera.nearPlane);
					ImGui::DragFloat("Far Plane", &_camera.farPlane);

					ImGui::TreePop();
				}
			}
		}
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

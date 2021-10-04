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
#include "MovementComponent.h"
#include "CameraComponent.h"
//#include "AudioSystem.h"
#include "GameObject.h"

#include "Time.h"

cs::VulkanEngineRenderer cs::ChinaEngine::renderer;
std::vector<cs::MeshRenderer*> cs::ChinaEngine::objects;

void cs::ChinaEngine::Run()
{
	Time::CycleInit();

	EngineInit();

	renderer.Create(800, 600, "China Speedrun");

	ResourceManager::InitializeTest();

	// Temporary solution to a visual glitch
	renderer.Redraw();

	InitInput();
	ImGuiStyleInit();

	MainLoop();
	EngineExit();
}

std::vector<cs::MeshRenderer*> const& cs::ChinaEngine::GetObjects()
{
	return objects;
}

cs::MeshRenderer* cs::ChinaEngine::InstanceObject(Mesh* mesh, Material* material, const Vector3 position)
{
	if (mesh == nullptr)
		return nullptr;

	MeshRenderer* _newObject{ new MeshRenderer };

	// for now we just push one material, but we'll add support for more later
	_newObject->mesh = mesh;
	_newObject->materials.push_back(material);
	_newObject->uboOffset = sizeof(UniformBufferObject) * objects.size(); // haha.. we won't do this in the future... what if we wanted to delete objects... it would be devastating
	_newObject->ubo->model = glm::translate(Matrix4x4(1.0f), position);
	_newObject->ubo->view = glm::lookAt(Vector3(2.0f, 2.0f, 2.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));

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
	Material* _material{ ResourceManager::Load<Material>("./Resources/materials/test.mat") };

	// these make no difference when spawning an object... yet
	// meaning you can't assign these textures to any models...
	Texture* _vargFlush{ ResourceManager::Load<Texture>("../Resources/textures/varg_flush.png") };
	Texture* _junkoGyate{ ResourceManager::Load<Texture>("../Resources/textures/junko_gyate.png") };

	Mesh* _mesh1{ Mesh::CreateDefaultCube({0.1f, 0.1f, 1.0f}) };
	Mesh* _mesh2{ Mesh::CreateDefaultPlane({0.5f, 0.5f}) };
	Mesh* _mesh3{ ResourceManager::Load<Mesh>("../Resources/models/suzanne.obj") };

	// for now we're just creating objects like this... will change this in the future
	MeshRenderer* _obj1{ InstanceObject(_mesh1, _material, Vector3(-1.3f, 0.0f, 1.2f)) };
	MeshRenderer* _obj2{ InstanceObject(_mesh2, _material, Vector3(-0.45f, 0.7f, 0.0f)) };
	MeshRenderer* _obj3{ InstanceObject(_mesh3, _material, Vector3(0.0f, 1.0f, 0.0f)) };

	_obj1->active = true;
	_obj2->active = true;
	_obj3->active = true;

	_obj1->name = "Cube";
	_obj2->name = "Plane";
	_obj3->name = "Suzanne";
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
			for (MeshRenderer* object : objects)
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

		//world->Step();

		for (size_t i{ 0 }; i < objects.size(); i++)
			objects[i]->Update(i);

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

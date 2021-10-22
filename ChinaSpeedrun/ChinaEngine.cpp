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

#include "ImGuizmo.h"

#include "Input.h"
#include "World.h"
#include "Transform.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "Camera.h"
#include "PhysicsServer.h"

#include "Time.h"

#include "Debug.h"

cs::World cs::ChinaEngine::world;
cs::VulkanEngineRenderer cs::ChinaEngine::renderer;
std::vector<cs::GameObject*> cs::ChinaEngine::objects;

void cs::ChinaEngine::Run()
{
	Time::CycleInit();

	// ok so we need to fizzle out what needs to be done.
	// The order of things needs to be ordered
	// The VulkenRenderer needs to init the essentials, wilst also init the other things that
	// currently can be static.
	// We then can gain the ability to dynamically assign stuff

	EngineInit();
	renderer.Create(800, 600, "China Speedrun");

	ResourceManager::InstanceAllResources();

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
	_shader->AssignShaderVertexInputAttrib("position", 0, Shader::Data::VEC3, offsetof(Vertex, position));
	_shader->AssignShaderVertexInputAttrib("color", 1, Shader::Data::VEC3, offsetof(Vertex, color));
	_shader->AssignShaderVertexInputAttrib("texCoord", 2, Shader::Data::VEC2, offsetof(Vertex, texCoord));

	_shader->AssignShaderDescriptor("ubo", 0, Shader::Type::VERTEX, Shader::Data::UNIFORM);
	_shader->AssignShaderDescriptor("texSampler", 1, Shader::Type::FRAGMENT, Shader::Data::SAMPLER2D);

	Material* _material{ ResourceManager::Load<Material>("../Resources/materials/test.mat") };
	Material* _material1{ ResourceManager::Load<Material>("../Resources/materials/test1.mat") };
	Material* _material2{ ResourceManager::Load<Material>("../Resources/materials/test2.mat") };
	_material->shader = _shader;
	_material1->shader = _shader;
	_material2->shader = _shader;
	_material->renderMode = Material::RenderMode::OPEQUE_; // This is not nesseccary, it is set to Opeque by default
	_material->fillMode = Material::FillMode::FILL;
	_material->cullMode = Material::CullMode::BACK;

	// these make no difference when spawning an object... yet
	// meaning you can't assign these textures to any models...
	Texture* _vargFlush{ ResourceManager::Load<Texture>("../Resources/textures/varg_flush.png") };
	Texture* _junkoGyate{ ResourceManager::Load<Texture>("../Resources/textures/junko_gyate.png") };
	Texture* _chaikaSmile{ ResourceManager::Load<Texture>("../Resources/textures/chaika_smile.png") };

	//_material->shaderParams["texSampler"] = _vargFlush;
	//_material1->shaderParams["texSampler"] = _junkoGyate;
	//_material2->shaderParams["texSampler"] = _chaikaSmile;
	//_material->shaderParams["time"] = 0.0f;
	
	// for now we're just creating objects like this... will change this in the future
	GameObject* _cube{ InstanceObject("Cube", Vector3(-1.3f, 0.0f, 1.2f)) };
	GameObject* _plane{ InstanceObject("Plane", Vector3(-0.45f, 0.7f, 0.0f)) };
	GameObject* _plane2{ InstanceObject("Plane2", Vector3(0.75f, -0.3f, 2.0f)) };
	GameObject* _suzanne{ InstanceObject("Suzanne", Vector3(0.0f, -2.0f, -1.0f), Vector3(90.0f, 0.0f, 0.0f)) };
	GameObject* _randomObject{ InstanceObject("This object has no renderer") };
	GameObject* _camera{ InstanceObject("Camera", Vector3(0.0f, 1.0f, -6.0f), Vector3(62.0f, -23.0f, 16.0f)) };

	MeshRendererComponent& _meshRendererCube{ _cube->AddComponent<MeshRendererComponent>() };
	_meshRendererCube.mesh = Mesh::CreateDefaultCube({ 0.1f, 0.1f, 1.0f });
	_meshRendererCube.materials.push_back(_material);

	MeshRendererComponent& _meshRendererPlane{ _plane->AddComponent<MeshRendererComponent>() };
	_meshRendererPlane.mesh = Mesh::CreateDefaultPlane({ 0.5f, 0.5f });
	_meshRendererPlane.materials.push_back(_material2);

	MeshRendererComponent& _meshRendererPlane2{ _plane2->AddComponent<MeshRendererComponent>() };
	_meshRendererPlane2.mesh = _meshRendererPlane.mesh;
	_meshRendererPlane2.materials.push_back(_material1);

	MeshRendererComponent& _meshRendererMonke{ _suzanne->AddComponent<MeshRendererComponent>() };
	_meshRendererMonke.mesh = ResourceManager::Load<Mesh>("../Resources/models/suzanne.obj");
	_meshRendererMonke.materials.push_back(_material1);

	CameraComponent& _cameraComponent{ _camera->AddComponent<CameraComponent>() };
	CameraComponent::currentActiveCamera = &_cameraComponent;

	/*auto* _audioComponent{ &_cube->AddComponent<AudioComponent>() };
	_audioComponent->soundName = "koto";

	_audioComponent = &_suzanne->AddComponent<AudioComponent>();
	_audioComponent->soundName = "kazeoto";*/
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
	Matrix4x4 _viewMatrix{ glm::inverse(Camera::GetViewMatrix(*CameraComponent::currentActiveCamera)) }, _projectionMatrix{ Camera::GetProjectionMatrix(*CameraComponent::currentActiveCamera) }, _identityMatrix{ Matrix4x4(1.0f) };

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	int _width, _height;
	renderer.GetViewportSize(_width, _height);

	ImGuizmo::SetRect(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height));
	ImGuizmo::DrawGrid(glm::value_ptr(_viewMatrix), glm::value_ptr(_projectionMatrix), glm::value_ptr(_identityMatrix), 100.0f);

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
				//ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, static_cast<float>(ImGui::GetWindowWidth()), static_cast<float>(ImGui::GetWindowHeight()));

				TransformComponent& _transform{ _activeObject->GetComponent<TransformComponent>() };
				Matrix4x4 _mainMatrix{ _transform };

				ImGuizmo::Manipulate(glm::value_ptr(_viewMatrix), glm::value_ptr(_projectionMatrix),
					ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(_mainMatrix));
				
				if (ImGuizmo::IsUsing())
				{
					_transform.position = _mainMatrix[3];
				}

				if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::DragFloat3("Position", &_transform.position.x, 0.1f);
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

			if (_activeObject->HasComponent<SphereColliderComponent>())
			{
				if (ImGui::TreeNodeEx("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen))
				{
					SphereColliderComponent& _sphereCollider{ _activeObject->GetComponent<SphereColliderComponent>() };

					ImGui::DragFloat("Radius", &_sphereCollider.radius, 0.01f);

					ImGui::TreePop();
				}
			}

			if (_activeObject->HasComponent<PolygonCollider>())
			{
				if (ImGui::TreeNodeEx("Polygon Collider", ImGuiTreeNodeFlags_DefaultOpen))
				{


					ImGui::TreePop();
				}
			}

			if (_activeObject->HasComponent<CameraComponent>())
			{
				if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen))
				{
					CameraComponent& _camera{ _activeObject->GetComponent<CameraComponent>() };

					const char* _options[]{ "Perspective", "Orthographic" };
					ImGui::ListBox("Projection", (int*)&_camera.projection, _options, 2);

					ImGui::DragFloat("Field of View", &_camera.fov, 1.0f, 0.1f, 179.0f);
					ImGui::DragFloat("Near Plane", &_camera.nearPlane, 1.0f, 0.001f);
					ImGui::DragFloat("Far Plane", &_camera.farPlane, 1.0f);

					ImGui::TreePop();
				}
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("Profiler"))
		ImGui::Text("Delta Time: %f", Time::deltaTime);
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

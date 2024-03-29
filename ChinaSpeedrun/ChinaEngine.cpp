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
#include "Transform.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "Camera.h"
#include "PhysicsComponent.h"
#include "Script.h"
#include "SphereCollider.h"
#include "StaticBody.h"
#include "Rigidbody.h"
#include "BulletManagerComponent.h"

#include "Editor.h"
#include "SceneManager.h"
//#include "EditorProfiler.h"

#include "Time.h"

cs::VulkanEngineRenderer cs::ChinaEngine::renderer;
cs::editor::EngineEditor cs::ChinaEngine::editor;

void cs::ChinaEngine::Run()
{
	Time::CycleInit();
	Mathf::InitRand();

	renderer.Create(800, 600, "China Speedrun");

	editor.Start();

	EngineInit();

	renderer.Resolve();

	InitInput();

	MainLoop();
	EngineExit();
}

float cs::ChinaEngine::AspectRatio()
{
	return renderer.AspectRatio();
}

void cs::ChinaEngine::FramebufferResizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	if (newWidth * newHeight > 0) // don't recalculate the perspective if the screen size is 0
		Camera::CalculateProjection(*SceneManager::mainCamera);
}

void cs::ChinaEngine::EngineInit()
{
	/*SceneManager::Load(SceneManager::CreateScene("Performance Test"));
	SceneManager::Load(SceneManager::CreateScene("Physics Test"));
	SceneManager::Load(SceneManager::CreateScene("Bullet Hell Test"));
	//SceneManager::Load(ResourceManager::Load<Scene>("../Resources/scenes/test_1.json"));

	SceneManager::SetCurrentFocusedScene(0);

	Shader* _shader{ ResourceManager::Load<Shader>("../Resources/shaders/default_shader") };
	_shader->AssignShaderVertexInputAttrib("position", 0, Shader::Data::VEC3, offsetof(Vertex, position));
	_shader->AssignShaderVertexInputAttrib("color", 1, Shader::Data::VEC3, offsetof(Vertex, color));
	_shader->AssignShaderVertexInputAttrib("texCoord", 2, Shader::Data::VEC2, offsetof(Vertex, texCoord));
	//_shader->AssignShaderVertexInputAttrib("normal", 3, Shader::Data::VEC3, offsetof(Vertex, normal));

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

	const uint16_t width{ 8 }, length{ 8 }, height{ 8 };
	Mesh* _sphereModel{ ResourceManager::Load<Mesh>("../Resources/models/sphere_model.obj") };

	for (size_t x{ 0 }; x < width; x++)
	{
		for (size_t y{ 0 }; y < height; y++)
		{
			for (size_t z{ 0 }; z < length; z++)
			{
				GameObject* _object{ SceneManager::InstanceObject(std::to_string(x + (y * 4) + (z * 16)).c_str(), Vector3((float)x, (float)y, (float)z) * 2.0f) };

				MeshRendererComponent& _terrainMesh{ _object->AddComponent<MeshRendererComponent>() };
				_terrainMesh.SetMesh(_sphereModel);
				_terrainMesh.material = _material1;
			}
		}
	}

	// PHYSICS TEST
	SceneManager::SetCurrentFocusedScene(1);

	GameObject* _terrain{ SceneManager::InstanceObject("Terrain", Vector3(0.0f, -6.0f, 0.0f)) };
	GameObject* _suzanne{ SceneManager::InstanceObject("Suzanne", Vector3(0.0f, 10.0f, 4.0f)) }; // Vector3(-7.0f, 5.0f, -6.2f) // -1.0f, 10.0f, 6.0f
	GameObject* _physicsBall{ SceneManager::InstanceObject("Junko Ball", Vector3(-1.3f, 3.0f, 5.5f)) };
	GameObject* _camera{ SceneManager::InstanceObject("Camera", Vector3(13.0f, 13.0f, 16.0f), Vector3(-33.0f, 35.0f, 0.0f)) };

	MeshRendererComponent& _terrainMesh{ _terrain->AddComponent<MeshRendererComponent>() };
	_terrainMesh.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/terrain.obj"));
	_terrainMesh.material = _material2;

	ScriptComponent& _terrainScript{ _terrain->AddComponent<ScriptComponent>() };
	_terrainScript.SetScript(ResourceManager::Load<Script>("../Resources/scripts/lua_test.lua"));

	SphereColliderComponent& _sphereColTerrain{ _terrain->AddComponent<SphereColliderComponent>() };
	_sphereColTerrain.radius = 10.0f;
	StaticBodyComponent& _rbT{ _terrain->AddComponent<StaticBodyComponent>() };

	MeshRendererComponent& _meshRendererMonke{ _suzanne->AddComponent<MeshRendererComponent>() };
	_meshRendererMonke.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/suzanne.obj"));
	_meshRendererMonke.material = _material1;
	
	_suzanne->AddComponent<SphereColliderComponent>();
	RigidbodyComponent& _rbZU{ _suzanne->AddComponent<RigidbodyComponent>() };
	_rbZU.mass = 1.0f;

	MeshRendererComponent& _junkoBall{ _physicsBall->AddComponent<MeshRendererComponent>() };
	_junkoBall.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/sphere_model.obj"));
	_junkoBall.material = _material1;
	
	_physicsBall->AddComponent<SphereColliderComponent>();
	RigidbodyComponent& _rbPB{ _physicsBall->AddComponent<RigidbodyComponent>() };

	_camera->AddComponent<CameraComponent>();*/

	SceneManager::Load(SceneManager::CreateScene("Bullet Hell Test"));

	// BULLET HELL TEST
	SceneManager::SetCurrentFocusedScene(0);

	GameObject* _bulletManager{ SceneManager::InstanceObject("Bullet Manager") };
	GameObject* _cameraBullet{ SceneManager::InstanceObject("Main Camera", Vector3(0.0f, 0.0f, -10.0f)) };
	//GameObject* _object{ SceneManager::InstanceObject("Relative Object", Vector3(1.0f, -2.0f, 6.0f)) };

	Shader* _bulletShader{ ResourceManager::Load<Shader>("../Resources/shaders/bullet_shader") };
	_bulletShader->AssignShaderVertexInputAttrib("position", 0, Shader::Data::VEC3);
	_bulletShader->AssignShaderVertexInputAttrib("color", 1, Shader::Data::VEC3);
	_bulletShader->AssignShaderVertexInputAttrib("texCoord", 2, Shader::Data::VEC2);
	_bulletShader->AssignShaderVertexBinding(Shader::InputRate::VERTEX);

	_bulletShader->AssignShaderVertexInputAttrib("positionOffset", 3, Shader::Data::VEC2);
	_bulletShader->AssignShaderVertexInputAttrib("rotation", 4, Shader::Data::FLOAT);
	_bulletShader->AssignShaderVertexInputAttrib("mainColor", 5, Shader::Data::VEC4);
	_bulletShader->AssignShaderVertexInputAttrib("subColor", 6, Shader::Data::VEC4);
	_bulletShader->AssignShaderVertexBinding(Shader::InputRate::INSTANCE);

	_bulletShader->AssignShaderDescriptor("ubo", 0, Shader::Type::VERTEX, Shader::Data::UNIFORM);
	_bulletShader->AssignShaderDescriptor("texMainSampler", 1, Shader::Type::FRAGMENT, Shader::Data::SAMPLER2D);
	_bulletShader->AssignShaderDescriptor("texSubSampler", 2, Shader::Type::FRAGMENT, Shader::Data::SAMPLER2D);
	
	Material* _bulletMaterial{ ResourceManager::Load<Material>("../Resources/materials/bullet.mat") };
	_bulletMaterial->shader = _bulletShader;
	_bulletMaterial->renderMode = Material::RenderMode::TRANSPARENT_;
	_bulletMaterial->cullMode = Material::CullMode::NONE;

	Texture* _mainTexture{ ResourceManager::Load<Texture>("../Resources/textures/bullet_circle_main.png") };
	Texture* _subTexture{ ResourceManager::Load<Texture>("../Resources/textures/bullet_circle_sub.png") };

	_mainTexture->filter = Texture::Filter::NEAREST;
	_subTexture->filter = Texture::Filter::NEAREST;
	_mainTexture->generateMipmaps = true;
	_subTexture->generateMipmaps = false;

	_bulletMaterial->shaderParams["texMainSampler"] = _mainTexture;
	_bulletMaterial->shaderParams["texSubSampler"] = _subTexture;

	BulletManagerComponent& _manager{ _bulletManager->AddComponent<BulletManagerComponent>() };
	_manager.material = _bulletMaterial;
	_manager.bulletCapacity = 10000;
	_manager.CreateBorders(1.6f, 1.8f, 0.0f);
	_manager.CreateSystem();

	CameraComponent& _bulletCam{ _cameraBullet->AddComponent<CameraComponent>() };
	_bulletCam.projection = CameraComponent::Projection::ORTHOGRAPHIC;
	_bulletCam.SetExtents(1920.0f, 1080.0f);
	Camera::CalculateProjection(_bulletCam);

	//MeshRendererComponent& _objectMeshRenderer{ _object->AddComponent<MeshRendererComponent>() };
	//_objectMeshRenderer.SetMesh(Mesh::CreateDefaultPlane(Vector2(0.5f)));
	//_objectMeshRenderer.material = _material2;
}

void cs::ChinaEngine::InitInput()
{
	glfwSetKeyCallback(renderer.GetWindow(), Input::GlfwKeyfunCallback);
	glfwSetCursorPosCallback(renderer.GetWindow(), Input::GlfwCursorPosCallback);
	glfwSetScrollCallback(renderer.GetWindow(), Input::GlfwScrollCallback);
	glfwSetMouseButtonCallback(renderer.GetWindow(), Input::GlfwMouseButtonCallback);

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
		SceneManager::Resolve();

		glfwPollEvents();

		editor.Update();
		SceneManager::Update();

		renderer.Resolve();
		renderer.DrawFrame();
		Input::FinishFrame();

		Time::CycleEnd();
	}

	vkDeviceWaitIdle(renderer.GetDevice());
}

void cs::ChinaEngine::EngineExit()
{
	editor.Exit();
	ResourceManager::ClearAllResourcePools();
	SceneManager::DestroyEverything();
}

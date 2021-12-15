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
#include "BSpline.h"
#include "Camera.h"
#include "PhysicsComponent.h"
#include "PolygonCollider.h"
#include "SphereCollider.h"
#include "StaticBody.h"
#include "Rigidbody.h"
#include "Script.h"
#include "BulletManagerComponent.h"
#include "DirectionalLight.h"
#include "SpriteRenderer.h"
#include "Timer.h"

#include "Editor.h"
#include "Draw.h"
#include "SceneManager.h"

#include "Time.h"

cs::VulkanEngineRenderer cs::ChinaEngine::renderer;
cs::editor::EngineEditor cs::ChinaEngine::editor;

void cs::ChinaEngine::Run()
{
	Time::CycleInit();
	Mathf::InitRand();

	editor::EditorOptions::LoadSettings();

	renderer.Create(editor::EditorOptions::windowWidth, editor::EditorOptions::windowHeight, "China Speedrun");

	editor.Start();
	Draw::Setup();
	renderer.Resolve();
	Draw::CreateDescriptorSets();
	Draw::DebugGrid();

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
	SceneManager::Load(SceneManager::CreateScene("Test Scene"));
	SceneManager::SetCurrentFocusedScene(0);

	Shader* _shader{ ResourceManager::Load<Shader>("../Resources/shaders/default_shader") };
	_shader->AssignShaderVertexInputAttrib("position", 0, Shader::Data::VEC3);
	_shader->AssignShaderVertexInputAttrib("color", 1, Shader::Data::VEC3);
	_shader->AssignShaderVertexInputAttrib("texCoord", 2, Shader::Data::VEC2);
	//_shader->AssignShaderVertexInputAttrib("normal", 3, Shader::Data::VEC3);
	_shader->AssignShaderVertexBinding(Shader::InputRate::VERTEX);

	_shader->AssignShaderDescriptor("ubo", 0, Shader::Type::VERTEX, Shader::Data::UNIFORM);
	_shader->AssignShaderDescriptor("texSampler", 1, Shader::Type::FRAGMENT, Shader::Data::SAMPLER2D);

	Shader* _lavaFlowShader{ ResourceManager::Load<Shader>("../Resources/shaders/lava_moving") };
	_lavaFlowShader->AssignShaderVertexInputAttrib("position", 0, Shader::Data::VEC3);
	_lavaFlowShader->AssignShaderVertexInputAttrib("color", 1, Shader::Data::VEC3);
	_lavaFlowShader->AssignShaderVertexInputAttrib("texCoord", 2, Shader::Data::VEC2);
	_lavaFlowShader->AssignShaderVertexBinding(Shader::InputRate::VERTEX);

	_lavaFlowShader->AssignShaderDescriptor("ubo", 0, Shader::Type::VERTEX, Shader::Data::UNIFORM);
	_lavaFlowShader->AssignShaderDescriptor("texSampler", 1, Shader::Type::FRAGMENT, Shader::Data::SAMPLER2D);
	_lavaFlowShader->AssignShaderDescriptor("TIME", 2, Shader::Type::FRAGMENT, Shader::Data::UNIFORM);
	
	Texture* _debugTexture{ ResourceManager::Load<Texture>("../Resources/textures/debug_texture.png") };
	_debugTexture->filter = Texture::Filter::NEAREST;

	Texture* _magma{ ResourceManager::Load<Texture>("../Resources/textures/magma.png") };
	_magma->tilingX = Texture::Tiling::MIRRORED_REPEAT;
	_magma->tilingY = Texture::Tiling::MIRRORED_REPEAT;

	Material* _material{ ResourceManager::Load<Material>("../Resources/materials/test1.mat") };
	Material* _magmaMaterial{ ResourceManager::Load<Material>("../Resources/materials/test2.mat") };

	_material->shader = _shader;
	_material->shaderParams["texSampler"] = _debugTexture;

	_magmaMaterial->shader = _lavaFlowShader;
	//_magmaMaterial->cullMode = Material::CullMode::NONE;
	_magmaMaterial->shaderParams["texSampler"] = _magma;

	{
		GameObject* _gameObject{ SceneManager::InstanceObject("Parent Object") };
		GameObject* _gameObject2{ SceneManager::InstanceObject("Child Object", Vector3(10.0f, 10.0f, 10.0f)) };

		MeshRendererComponent& _suzanne{ _gameObject->AddComponent<MeshRendererComponent>() };
		_suzanne.material = _material;
		_suzanne.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/suzanne.obj"));
		_gameObject->AddComponent<Timer>();
		_gameObject->AddComponent<BSpline>();

		MeshRendererComponent& _sphere{ _gameObject2->AddComponent<MeshRendererComponent>() };
		_sphere.material = _magmaMaterial;
		_sphere.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/geothermal_mantle.obj"));

		_gameObject->AddChild(_gameObject2);
	}
	
	{
		// BULLET HELL TEST
		SceneManager::Load(SceneManager::CreateScene("Bullet Hell Test"));
		SceneManager::SetCurrentFocusedScene(1);

		GameObject* _bulletManager{ SceneManager::InstanceObject("Bullet Manager") };
		GameObject* _player{ SceneManager::InstanceObject("Player") };
		GameObject* _cameraBullet{ SceneManager::InstanceObject("Main Camera", Vector3(0.0f, 0.0f, -10.0f)) };

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
		//_bulletMaterial->cullMode = Material::CullMode::FRONT;

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
		_manager.bulletCapacity = 20000;
		_manager.CreateBorders(1.5f, 1.5f, 0.0f);
		_manager.CreateSystem();

		CameraComponent& _bulletCam{ _cameraBullet->AddComponent<CameraComponent>() };
		_bulletCam.projection = CameraComponent::Projection::ORTHOGRAPHIC;
		_bulletCam.SetExtents(200.0f, 200.0f);
		Camera::CalculateProjection(_bulletCam);

		_player->AddComponent<SpriteRenderer>();
		ScriptComponent& _playerScript{ _player->AddComponent<ScriptComponent>() };
		_playerScript.SetScript(ResourceManager::Load<Script>("../Resources/scripts/PlayerController.lua"));
	}
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
		Draw::Update();
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
	editor::EditorOptions::SaveSettings();
}

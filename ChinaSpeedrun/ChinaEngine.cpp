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

#include "Delaunay.h"
#include "MarchingTriangles.h"

#include "Editor.h"
#include "Draw.h"
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
	SceneManager::Load(SceneManager::CreateScene("Task 3.1 - 3.3"));
	SceneManager::Load(SceneManager::CreateScene("Task 3.4"));
	SceneManager::SetCurrentFocusedScene(0);

	Shader* _shader{ ResourceManager::Load<Shader>("../Resources/shaders/default_shader") };
	_shader->AssignShaderVertexInputAttrib("position", 0, Shader::Data::VEC3);
	_shader->AssignShaderVertexInputAttrib("color", 1, Shader::Data::VEC3);
	_shader->AssignShaderVertexInputAttrib("texCoord", 2, Shader::Data::VEC2);
	//_shader->AssignShaderVertexInputAttrib("normal", 3, Shader::Data::VEC3);
	_shader->AssignShaderVertexBinding(Shader::InputRate::VERTEX);

	_shader->AssignShaderDescriptor("ubo", 0, Shader::Type::VERTEX, Shader::Data::UNIFORM);
	_shader->AssignShaderDescriptor("texSampler", 1, Shader::Type::FRAGMENT, Shader::Data::SAMPLER2D);
	//_shader->AssignShaderDescriptor("TIME", 2, Shader::Type::ANY, Shader::Data::UNIFORM);

	Texture* _debugTexture{ ResourceManager::Load<Texture>("../Resources/textures/debug_texture.png") };
	_debugTexture->filter = Texture::Filter::NEAREST;

	Texture* _magma{ ResourceManager::Load<Texture>("../Resources/textures/magma.png") };
	_magma->tilingX = Texture::Tiling::MIRRORED_REPEAT;
	_magma->tilingY = Texture::Tiling::MIRRORED_REPEAT;

	Material* _material{ ResourceManager::Load<Material>("../Resources/materials/test1.mat") };
	Material* _magmaMaterial{ ResourceManager::Load<Material>("../Resources/materials/test2.mat") };

	_material->shader = _shader;
	_material->shaderParams["texSampler"] = _debugTexture;

	_magmaMaterial->shader = _shader;
	//_magmaMaterial->cullMode = Material::CullMode::NONE;
	_magmaMaterial->shaderParams["texSampler"] = _magma;

	{
		GameObject* _terrain{ SceneManager::InstanceObject("Delaunay Terrain", Vector3(0.0f, 0.0f, 0.0f)) };
		GameObject* _ball{ SceneManager::InstanceObject("Ball", Vector3(-4.0f, 10.0f, 2.0f)) };
		GameObject* _ball2{ SceneManager::InstanceObject("Ball", Vector3(-3.0f, 8.0f, -2.0f)) };
		GameObject* _light{ SceneManager::InstanceObject("Diractional Light", Vector3(0.0f), Vector3(0.0f, 40.0f, -10.0f)) };
		GameObject* _camera{ SceneManager::InstanceObject("Camera", Vector3(14.0f, 10.0f, 15.0f), Vector3(-25.0f, 42.5f, 0.0f)) };
		GameObject* _curves{ SceneManager::InstanceObject("Curves") };

		MeshRendererComponent& _meshRenderer{ _terrain->AddComponent<MeshRendererComponent>() };
		_meshRenderer.material = _magmaMaterial;

		std::vector<Vector3> _points{ ResourceManager::LoadLAS("../Resources/test_las.txt", Vector3(615200.0f, 6758300.0f, 550.0f)) }; // 245592
		/*BSpline& _bSpline{ _terrain->AddComponent<BSpline>() };
		_bSpline.points.push_back({ { 3.0f, 0.0f, 0.0f }, { 0.7f, 1.2f, 3.2f } });
		_bSpline.points.push_back({ { 0.0f, -4.0f, 2.0f }, { 7.3f, 5.0f, -3.2f } });
		_bSpline.points.push_back({ { -2.0f, 3.0f, -10.0f }, { -0.7f, -3.0f, 5.2f } });
		_bSpline.MakeDrawLine();*/

		//_meshRenderer.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/terrain.obj"));
		_meshRenderer.SetMesh(algorithm::Delaunay::Triangulate(_points, true));
		//ScriptComponent& _script{ _terrain->AddComponent<ScriptComponent>() };
		//_script.SetScript(ResourceManager::Load<Script>("../Resources/scripts/lua_test.lua"));

		DirectionalLight& _directionalLight{ _light->AddComponent<DirectionalLight>() };
		_directionalLight.MakeRayLines();
		
		_terrain->AddComponent<PolygonColliderComponent>();
		_terrain->AddComponent<StaticBodyComponent>();

		/* -------------------
		*  BALL AND BALL2
		*/

		_ball->AddComponent<SphereColliderComponent>();
		_ball->AddComponent<RigidbodyComponent>();

		MeshRendererComponent& _ballMesh{ _ball->AddComponent<MeshRendererComponent>() };
		_ballMesh.material = _material;
		_ballMesh.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/sphere_model.obj"));


		_ball2->AddComponent<SphereColliderComponent>();
		_ball2->AddComponent<RigidbodyComponent>();

		MeshRendererComponent& _ball2Mesh{ _ball2->AddComponent<MeshRendererComponent>() };
		_ball2Mesh.material = _material;
		_ball2Mesh.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/sphere_model.obj"));

		/* -------------------
		*  CAMERA
		*/

		_camera->AddComponent<CameraComponent>();

		/* -------------------
		*  HEIGHT CURVES
		*/

		algorithm::MarchingTriangles::Polygonize(_curves, _terrain->GetComponent<MeshRendererComponent>().mesh);
	}
	
	{
		SceneManager::SetCurrentFocusedScene(1);

		GameObject* _ball{ SceneManager::InstanceObject("Ball") };
		GameObject* _camera{ SceneManager::InstanceObject("Camera", Vector3(0.0f, 10.0f, 30.0f), Vector3(0.0f, 0.0f, 0.0f)) };

		_ball->AddComponent<SphereColliderComponent>();
		RigidbodyComponent& _rb{ _ball->AddComponent<RigidbodyComponent>() };
		_rb.mass = 1.0f;
		_rb.startForce = Vector3(4.0f, 20.0f, 0.0f);
		_rb.endTime = 3.0f;

		BSpline& _spline{ _ball->AddComponent<BSpline>() };
		_spline.subdivisions = 100;
		_spline.points.resize(2);
		_spline.MakeDrawLine();

		MeshRendererComponent& _meshRend{ _ball->AddComponent<MeshRendererComponent>() };
		_meshRend.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/sphere_model.obj"));
		_meshRend.material = Draw::material;

		_camera->AddComponent<CameraComponent>();
	}

	/*SceneManager::Load(SceneManager::CreateScene("Performance Test"));
	SceneManager::Load(SceneManager::CreateScene("Physics Test"));
	//SceneManager::Load(ResourceManager::Load<Scene>("../Resources/scenes/test_1.json"));

	SceneManager::SetCurrentFocusedScene(0);

	const uint16_t width{ 2 }, length{ 2 }, height{ 2 };
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
	GameObject* _physicsBall{ SceneManager::InstanceObject("Junko Ball", Vector3(-1.3f, 2.0f, 5.5f)) };
	GameObject* _camera{ SceneManager::InstanceObject("Camera", Vector3(13.0f, 13.0f, 16.0f), Vector3(-33.0f, 35.0f, 0.0f)) };

	MeshRendererComponent& _terrainMesh{ _terrain->AddComponent<MeshRendererComponent>() };
	_terrainMesh.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/geothermal_mantle.obj"));
	_terrainMesh.material = _material2;

	ScriptComponent& _terrainScript{ _terrain->AddComponent<ScriptComponent>() };
	_terrainScript.SetScript(ResourceManager::Load<Script>("../Resources/scripts/lua_test.lua"));

	PolygonColliderComponent& _sphereColTerrain{ _terrain->AddComponent<PolygonColliderComponent>() };
	StaticBodyComponent& _rbT{ _terrain->AddComponent<StaticBodyComponent>() };

	MeshRendererComponent& _meshRendererMonke{ _suzanne->AddComponent<MeshRendererComponent>() };
	_meshRendererMonke.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/suzanne.obj"));
	_meshRendererMonke.material = _material1;
	
	_suzanne->AddComponent<SphereColliderComponent>();
	RigidbodyComponent& _rbZU{ _suzanne->AddComponent<RigidbodyComponent>() };

	MeshRendererComponent& _junkoBall{ _physicsBall->AddComponent<MeshRendererComponent>() };
	_junkoBall.SetMesh(ResourceManager::Load<Mesh>("../Resources/models/sphere_model.obj"));
	_junkoBall.material = _material1;
	
	_physicsBall->AddComponent<SphereColliderComponent>();
	RigidbodyComponent& _rbPB{ _physicsBall->AddComponent<RigidbodyComponent>() };

	_camera->AddComponent<CameraComponent>();*/

	/*SceneManager::Load(SceneManager::CreateScene("Bullet Hell Test"));

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
	_manager.bulletCapacity = 20000;
	_manager.CreateBorders(1.6f, 1.8f, 0.0f);
	_manager.CreateSystem();

	CameraComponent& _bulletCam{ _cameraBullet->AddComponent<CameraComponent>() };
	_bulletCam.projection = CameraComponent::Projection::ORTHOGRAPHIC;
	_bulletCam.SetExtents(1920.0f, 1080.0f);
	Camera::CalculateProjection(_bulletCam);*/

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
}

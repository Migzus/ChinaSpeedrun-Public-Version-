#include "ChinaEngine.h"

#include <tiny_obj_loader.h>

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
#include "AudioSystem.h"

#include "Time.h"

using namespace cs;

//World* ChinaEngine::world;
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

void ChinaEngine::Run()
{
	Time::CycleInit();

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

MeshRenderer* ChinaEngine::InstanceObject(Mesh* mesh, Material* material, const Vector3 position)
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

	meshes.push_back(mesh);
	objects.push_back(_newObject);

	return _newObject;
}

float cs::ChinaEngine::AspectRatio()
{
	return renderer.AspectRatio();
}

Mesh* cs::ChinaEngine::IsDuplicateMesh(std::string filename)
{
	for (auto mesh : meshes)
		if (filename == mesh->GetResourcePath())
			return mesh;

	return nullptr;
}

Mesh* cs::ChinaEngine::LoadOBJ(std::string filename)
{
	Mesh* _outMesh{ IsDuplicateMesh(filename) };

	if (_outMesh == nullptr)
	{
		_outMesh = new Mesh;

		tinyobj::attrib_t _attributes;
		std::vector<tinyobj::shape_t> _shapes;
		std::vector<tinyobj::material_t> _materials;
		std::string _warning, _error;

		if (!tinyobj::LoadObj(&_attributes, &_shapes, &_materials, &_warning, &_error, filename.c_str()))
		{
			std::cout << "[ERROR]\t: " << _warning + _error << '\n';
			delete _outMesh;
			return nullptr;
		}

		std::vector<Vertex> _vertices;
		std::vector<uint32_t> _indices;
		// by using an unordered_map we can remove duplicates
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : _shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex _vertex{};

				_vertex.position =
				{
					_attributes.vertices[3 * index.vertex_index + 0],
					_attributes.vertices[3 * index.vertex_index + 1],
					_attributes.vertices[3 * index.vertex_index + 2]
				};

				_vertex.texCoord =
				{
					_attributes.texcoords[2 * index.texcoord_index + 0],
					1.0f - _attributes.texcoords[2 * index.texcoord_index + 1]
				};

				_vertex.color = { 1.0f, 1.0f, 1.0f };

				//_vertices.push_back(_vertex);
				//_indices.push_back(_indices.size());

				// 93,312 verts
				// 18,176 verts
				// we avoid 75,136 vertices on suzanne's model

				if (uniqueVertices.count(_vertex) == 0)
				{
					uniqueVertices[_vertex] = static_cast<uint32_t>(_vertices.size());
					_vertices.push_back(_vertex);
				}

				_indices.push_back(uniqueVertices[_vertex]);
			}
		}

		_outMesh->SetMesh(_vertices, _indices);
	}

	return _outMesh;
}

void ChinaEngine::EngineInit()
{
	//world = new World;

	Shader* _shader{ new Shader({ "../Resources/shaders/vert.spv", "../Resources/shaders/frag.spv" }) };
	Material* _material{ new Material(_shader) };

	// these make no difference when spawning an object... yet
	// meaning you can't assign these textures to any models...
	Texture* _vargFlush{ new Texture("../Resources/textures/varg_flush.png") };
	Texture* _junkoGyate{ new Texture("../Resources/textures/junko_gyate.png") };

	Mesh* _mesh1{ Mesh::CreateDefaultCube({0.1f, 0.1f, 1.0f}) };
	Mesh* _mesh2{ Mesh::CreateDefaultPlane({0.5f, 0.5f}) };
	Mesh* _mesh3{ LoadOBJ("../Resources/models/suzanne.obj") };

	// for now we're just creating objects like this... will change this in the future
	MeshRenderer* _obj1{ InstanceObject(_mesh1, _material, Vector3(-1.3f, 0.0f, 1.2f)) };
	MeshRenderer* _obj2{ InstanceObject(_mesh2, _material, Vector3(-0.45f, 0.7f, 0.0f)) };
	MeshRenderer* _obj3{ InstanceObject(_mesh3, _material, Vector3(0.0f, 1.0f, 0.0f)) };

	_obj1->active = true;
	_obj2->active = false;
	_obj3->active = true;

	Texture* _texture{ ResourceManager::Load<Texture>("../Resources/textures/varg_flush.png") };
	Mesh* _mesh{ ResourceManager::Load<Mesh>("../Resources/textures/varg_flush.png") };
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

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();

		//world->Step();

		for (size_t i{ 0 }; i < objects.size(); i++)
			objects[i]->Update(i);

		ImGui::Render();
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

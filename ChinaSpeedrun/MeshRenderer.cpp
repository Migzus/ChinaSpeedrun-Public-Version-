#include "MeshRenderer.h"

#include "Mesh.h"
#include "ChinaEngine.h"

#include <chrono>

cs::MeshRenderer::MeshRenderer() :
    mesh{ nullptr }, ubo{ new UniformBufferObject }
{}

cs::MeshRenderer::MeshRenderer(Mesh* newMesh) :
    mesh{ newMesh }, ubo{ new UniformBufferObject }
{}

const std::vector<VkDescriptorSet>& cs::MeshRenderer::GetDescriptorSet() const
{
    return descriptorSets;
}

void cs::MeshRenderer::Update(size_t index)
{
	static auto _startTime{ std::chrono::high_resolution_clock::now() };

	auto _currentTime{ std::chrono::high_resolution_clock::now() };
	float _time{ std::chrono::duration<float, std::chrono::seconds::period>(_currentTime - _startTime).count() };

	// haha... we're not going to do it like this for every object
	// when we attach a script component, then we can talk about
	// custom logic per entity (this is just a simple workaround)
	//if (index == 0)
	//	ubo->model = glm::rotate(Matrix4x4(1.0f), _time * glm::radians(90.0f), Vector3(0.0f, 1.0f, 1.0f));
	//else if (index == 1)
	//	ubo->model = glm::rotate(glm::translate(Matrix4x4(1.0f), Vector3(-0.4f, 0.0f, 0.0f)), _time * glm::radians(90.0f), Vector3(0.0f, 0.0f, -1.0f));

	ubo->proj = glm::perspective(Mathf::PI * 0.25f, ChinaEngine::AspectRatio(), 0.1f, 10.0f);
	ubo->proj[1][1] *= -1;
}

uint16_t cs::MeshRenderer::GetUBOSize() const
{
	return sizeof(UniformBufferObject);
}

cs::MeshRenderer::~MeshRenderer()
{
	delete ubo;
}

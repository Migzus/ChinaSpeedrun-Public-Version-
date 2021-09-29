#include "MeshRenderer.h"

#include "Mesh.h"
#include "ChinaEngine.h"
#include "Time.h"

cs::MeshRenderer::MeshRenderer() :
	active{ true }, mesh{ nullptr }, ubo{ new UniformBufferObject }
{}

cs::MeshRenderer::MeshRenderer(Mesh* newMesh) :
	active{ true }, mesh{ newMesh }, ubo{ new UniformBufferObject }
{}

const std::vector<VkDescriptorSet>& cs::MeshRenderer::GetDescriptorSet() const
{
    return descriptorSets;
}

void cs::MeshRenderer::Update(size_t index)
{
	// haha... we're not going to do it like this for every object
	// when we add support for a script component, then we can talk about
	// custom logic per entity (this is just a simple workaround)
	if (index == 0)
		ubo->model = glm::rotate(Matrix4x4(1.0f), Time::time * glm::radians(90.0f), Vector3(0.0f, 1.0f, 1.0f));
	else if (index == 1)
		ubo->model = glm::rotate(glm::translate(Matrix4x4(1.0f), Vector3(-0.4f, 1.0f, 0.0f)), Time::time * glm::radians(90.0f), Vector3(0.0f, 0.0f, -1.0f));
	else if (index == 2)
		ubo->model = glm::rotate(glm::translate(Matrix4x4(1.0f), Vector3(-2.0f, -2.0f, -1.0f)), Mathf::PI * 0.5f, Vector3(0.0f, 1.0f, 0.0f));

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

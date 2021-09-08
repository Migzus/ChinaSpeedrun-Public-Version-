#include "MeshRenderer.h"

cs::MeshRenderer::MeshRenderer() :
    mesh{ nullptr }
{}

cs::MeshRenderer::MeshRenderer(Mesh * newMesh) :
    mesh{ newMesh }
{}

const std::vector<VkDescriptorSet>& cs::MeshRenderer::GetDescriptorSet() const
{
    return descriptorSets;
}

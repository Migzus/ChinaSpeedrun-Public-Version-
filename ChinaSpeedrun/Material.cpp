#include "Material.h"

#include "ChinaEngine.h"
#include "VulkanEngineRenderer.h"

cs::Material::Material() :
	renderMode{ RenderMode::OPEQUE_ }, fillMode{ FillMode::FILL }, cullMode{ CullMode::BACK },
	lineWidth{ 1.0f }, shader{ nullptr }, enableStencil{ false }
{
	Initialize();
}

void cs::Material::Initialize()
{
	ChinaEngine::renderer.SolveMaterial(this, Solve::ADD);
}

void cs::Material::UpdateShaderParams()
{

}

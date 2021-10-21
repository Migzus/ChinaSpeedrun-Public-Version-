#include "Material.h"

#include "Shader.h"

cs::Material::Material() :
	renderMode{ RenderMode::OPEQUE_ }, fillMode{ FillMode::FILL }, cullMode{ CullMode::BACK }, lineWidth{ 1.0f }, shader{ nullptr }
{}

void cs::Material::Initialize()
{

}
